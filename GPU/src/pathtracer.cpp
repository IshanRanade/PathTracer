#include "pathtracer.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <thread>
#include <vector>
#include <stdlib.h>
#include "globals.h"


PathTracer::PathTracer(int imageWidth, int imageHeight) :
    imageWidth(imageWidth), imageHeight(imageHeight) {
}

void PathTracer::pickPlatform(cl::Platform& platform, const std::vector<cl::Platform>& platforms) {
    platform = platforms[0];
}

void PathTracer::pickDevice(cl::Device& device, const std::vector<cl::Device>& devices) {
    device = devices[1];
}

void PathTracer::printErrorLog(const cl::Program& program, const cl::Device& device) {
	// Get the error log and print to console
	std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
	std::cerr << "Build log:" << std::endl << buildlog << std::endl;
	exit(1);
}

void PathTracer::initOpenCL(cl::Device& device, cl::Context context, cl::CommandQueue& queue, cl::Program& program, cl::Kernel& kernel)
{
	// Get all available OpenCL platforms (e.g. AMD OpenCL, Nvidia CUDA, Intel OpenCL)
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	// Pick one platform
	cl::Platform platform;
	pickPlatform(platform, platforms);
	std::cout << "Using OpenCL platform: \t" << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;

	// Get available OpenCL devices on platform
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	// Pick one device
	pickDevice(device, devices);
	std::cout << "Using OpenCL device: \t" << device.getInfo<CL_DEVICE_NAME>() << std::endl;

	// // Create an OpenCL context on that device.
	context = cl::Context(device);

	// Create a command queue
	queue = cl::CommandQueue(context, device);

    //char *real_path = realpath(rel_path, NULL);

    std::ifstream file(KERNEL_ROOT_DIR + "kernel.cl");
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    program = cl::Program(context, source.c_str());
    cl_int result = program.build({device});
    if (result) std::cout << "\nError during compilation OpenCL code!!!\n (" << result << ")" << std::endl;
	if (result == CL_BUILD_PROGRAM_FAILURE) printErrorLog(program, device);
}

struct Ray {
    float origin[3];
    float direction[3];
};

std::vector<float> PathTracer::getFrameBuffer() {
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    cl::Kernel kernel;

    initOpenCL(device, context, queue, program, kernel);

    // Generate the initial camera rays
    kernel = cl::Kernel(program, "generateCameraRay");
    cl::Buffer rayBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, imageWidth * imageHeight * sizeof(Ray));
    kernel.setArg(0, rayBuffer);
	kernel.setArg(1, imageWidth);
	kernel.setArg(2, imageHeight);

    queue.enqueueNDRangeKernel(kernel, NULL, 16, 16);
	queue.finish();

    // Advance the rays
    // kernel = cl::Kernel(program, "generateCameraRays");

    // cl::Buffer cl_output = cl::Buffer(context, CL_MEM_WRITE_ONLY, imageWidth * imageHeight * sizeof(cl_float3));

    // kernel.setArg(0, cl_output);
	// kernel.setArg(1, imageWidth);
	// kernel.setArg(2, imageHeight);

    float* frameBufferData = (float*) malloc(sizeof(float) * imageWidth * imageHeight * 3);
    queue.enqueueReadBuffer(rayBuffer, false, 0, sizeof(float) * imageWidth * imageHeight * 3, frameBufferData, 0, NULL);
    
    std::vector<float> result;
    for(int i = 0; i < imageWidth * imageHeight * 3; i++) {
        result.push_back(frameBufferData[i]);
    }
    free(frameBufferData);

    std::cout << "done" << std::endl;

    return result;
}
