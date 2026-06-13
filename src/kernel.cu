#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <math.h>
#include <pathtracer/Scene.cuh>
#include <pathtracer/ray.cuh>
#include <pathtracer/vec3.cuh>

__device__ Vec3 color(Ray incomingRay, DeviceScene& deviceScene){
    /*
    For now, if we hit a sphere, color is red
    Otherwise, have some default sky color gradient

    TODO: Move object colors into the scene definition
    Add support for planes, triangles, environment maps? 
    */

    for (int i=0; i<deviceScene.nSpheres; i++){
        // TODO: Move sphere intersection logic into the sphere class
        Sphere& s = deviceScene.dSpheres[i];
        Vec3 p = s.center - incomingRay.A;
        Vec3 r = incomingRay.point_at_parameter(1);
        Vec3 dist = p - (p.dot(r)) * r.make_unit_vector();
        if (dist.squared_length() <= s.radius * s.radius){
            return Vec3(1.0, 0.0, 0.0);
        }
    }

    return Vec3(0.0, 0.0, 1.0);
}

__global__ void renderKernel(uchar4* output, int width, int height, float time, DeviceScene deviceScene) {  
    // Map 2D grid blocks directly to pixel coordinates
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // Core boundary check
    if (x >= width || y >= height) return;

    int index = y * width + x;
    
    // ABSOLUTE SAFETY HARNESS: Hard limits access to exact buffer boundaries
    if (index >= (width * height) || index < 0) return;

    Ray ray = deviceScene.dCamera.get_ray((float)x / width, (float)y / height);
    // printf("(%d, %d) --> (%.3f, %.3f) -->  (%.3f, %.3f, %.3f)\n", x, y, (float)x / width, (float)y / height, ray.direction().x, ray.direction().y, ray.direction().z);

    Vec3 col = color(ray, deviceScene);

    // Simple mathematical ray generation / color proxy
    float r = col.x;
    float g = col.y;
    float b = col.z;

    // Write calculations to the Managed Memory array
    output[index] = make_uchar4((int)(r * 255.0f), (int)(g * 255.0f), (int)(b * 255.0f), 255);
}

// Global C-Linkage wrapper function called by main.cpp
extern "C" void launch_kernel(uchar4* d_out, int width, int height, float time, DeviceScene deviceScene) {
    dim3 block(16, 16);
    
    // Clean ceiling math to make sure fractional edge blocks are rounded UP
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);
    renderKernel<<<grid, block>>>(d_out, width, height, time, deviceScene);
}