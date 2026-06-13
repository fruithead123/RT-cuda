#ifndef SCENE_H
#define SCENE_H

#include <pathtracer/Camera.cuh>
#include <pathtracer/Primitive.h>
#include <cuda_runtime.h>
#include <vector>

// Scene that is passed to the GPU
struct DeviceScene{
    Sphere* dSpheres;
    size_t nSpheres;
    Camera dCamera;
};

// Scene on the CPU
class Scene{
public:
    Scene() {};
    ~Scene() {freeDevice();}
    void addSphere(Vec3 enter, float radius);
    void uploadToDevice();
    void freeDevice();

    DeviceScene getDeviceScene() {
        return hDeviceScene;
    }

public:
    std::vector<Sphere> hSpheres;
    Camera hCamera;

    DeviceScene hDeviceScene;
    bool isUploaded = false;
};


#endif