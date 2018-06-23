#pragma once

#include "../include/cl.hpp"


/*
    This encapsulates all the logic for path tracing.  It exposes a primary public function, getFrameBuffer(),
    that uses its set values to path trace and image and return a buffer with the color values of the pixels
    in the image.
*/
class PathTracer {
public:
    PathTracer(int width, int height);
    std::vector<float> getFrameBuffer();

private:
    int imageWidth;
    int imageHeight;
    
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    cl::Kernel kernel;

    void pickPlatform(cl::Platform& platform, const std::vector<cl::Platform>& platforms);
    void pickDevice(cl::Device& device, const std::vector<cl::Device>& devices);
    void printErrorLog(const cl::Program& program, const cl::Device& device);
    void initOpenCL(cl::Device& device, cl::Context context, cl::CommandQueue& queue, cl::Program& program, cl::Kernel& kernel);
};
