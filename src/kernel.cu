#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <math.h>

__global__ void renderKernel(uchar4* output, int width, int height, float time) {
    // Map 2D grid blocks directly to pixel coordinates
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // Core boundary check
    if (x >= width || y >= height) return;

    int index = y * width + x;
    
    // ABSOLUTE SAFETY HARNESS: Hard limits access to exact buffer boundaries
    if (index >= (width * height) || index < 0) return;

    // Simple mathematical ray generation / color proxy
    float r = (float)x / width;
    float g = (float)y / height;
    float b = 0.5f + 0.5f * __sinf(time);

    // Write calculations to the Managed Memory array
    output[index] = make_uchar4((int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f), 255);
}

// Global C-Linkage wrapper function called by main.cpp
extern "C" void launch_kernel(uchar4* d_out, int width, int height, float time) {
    dim3 block(16, 16);
    
    // Clean ceiling math to make sure fractional edge blocks are rounded UP
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);

    renderKernel<<<grid, block>>>(d_out, width, height, time);
}