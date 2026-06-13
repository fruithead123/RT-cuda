#include <pathtracer/Scene.cuh>
#include <iostream>

#define CUDA_CHECK(ans) { cudaAssert((ans), __FILE__, __LINE__); }
inline void cudaAssert(cudaError_t code, const char *file, int line) {
   if (code != cudaSuccess) {
      std::cerr << "CUDA Error: " << cudaGetErrorString(code) << " in " << file << " at line " << line << std::endl;
      exit(code);
   }
}

void Scene::addSphere(Vec3 center, float radius){
    hSpheres.push_back({center, radius});
}

void Scene::uploadToDevice(){
    if (isUploaded) return;

    if (hSpheres.size() < 1){
        std::cerr << "Trying to upload an empty scene!" << std::endl;
        return;
    }

    // Set up the device scene
    hDeviceScene.dCamera = hCamera;
    hDeviceScene.nSpheres = hSpheres.size();

    size_t bytes = hDeviceScene.nSpheres * sizeof(Sphere);
    CUDA_CHECK(cudaMalloc(&hDeviceScene.dSpheres, bytes))
    CUDA_CHECK(cudaMemcpy(hDeviceScene.dSpheres, hSpheres.data(), bytes, cudaMemcpyHostToDevice));

    isUploaded = true;
}

void Scene::freeDevice(){
    if (!isUploaded) return;

    if (hDeviceScene.dSpheres != nullptr){
        CUDA_CHECK(cudaFree(hDeviceScene.dSpheres));
        hDeviceScene.dSpheres = nullptr;
    }
    hDeviceScene.nSpheres = 0;
    isUploaded = false;
}