#include "pathtracer.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <thread>
#include <vector>

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

    std::ifstream file("kernel.cl");
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    program = cl::Program(context, source.c_str());
    cl_int result = program.build({device});
    if (result) std::cout << "\nError during compilation OpenCL code!!!\n (" << result << ")" << std::endl;
	if (result == CL_BUILD_PROGRAM_FAILURE) printErrorLog(program, device);

    kernel = cl::Kernel(program, "render_kernel");
}

int PathTracer::getFrameBuffer() {
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    cl::Kernel kernel;

    initOpenCL(device, context, queue, program, kernel);

    int imageWidth = 100;
    int imageHeight = 100;
    int rendermode = 1;

    //cl::Buffer rayBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, imageWidth * imageHeight

    cl::Buffer cl_output = cl::Buffer(context, CL_MEM_WRITE_ONLY, imageWidth * imageHeight * sizeof(cl_float3));

    kernel.setArg(0, cl_output);
	kernel.setArg(1, imageWidth);
	kernel.setArg(2, imageHeight);
	kernel.setArg(3, rendermode);

    queue.enqueueNDRangeKernel(kernel, NULL, 16, 16);
	queue.finish();

    std::cout << "done" << std::endl;

    return 0;
}