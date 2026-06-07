#ifndef CAMERA_H
#define CAMERA_H

#include <pathtracer/ray.cuh>

class Camera{
    __device__ Camera(){
        lower_left = Vec3(-2.0, -1.0, -1.0);
        horizontal = Vec3(4.0, 0.0, 0.0);
        vertical = Vec3(0.0, 2.0, 0.0);
        origin = Vec3(0.0, 0.0, 0.0);
    }

    __device__ Ray get_ray(float u, float v) { return Ray(origin, lower_left + u*horizontal + v*vertical - origin); }


private:
    Vec3 lower_left;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 origin;
};

#endif