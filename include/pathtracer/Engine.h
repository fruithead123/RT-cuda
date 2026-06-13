#ifndef ENGINE_H
#define ENGINE_H

#include <pathtracer/Frontend.h>
#include <pathtracer/Scene.cuh>
#include <memory>

extern "C" void launch_kernel(uchar4* d_out, int width, int height, float time, DeviceScene deviceScene);

class Engine {
private:
    std::unique_ptr<Frontend> output;
    uchar4* dev_render_buffer;
    int WIDTH = 1280;
    int HEIGHT = 720;
    std::unique_ptr<Scene> scene = nullptr;

    // Profiling
    cudaEvent_t cudaStartTime, cudaEndTime;

    void init();

public:
    Engine(std::unique_ptr<Frontend> display) : output(std::move(display)) {}
    ~Engine();
    
    void setScreenSize(int width, int height){
        WIDTH = width;
        HEIGHT = height;
    }
    void setScene(std::unique_ptr<Scene> newScene) {
        scene = std::move(newScene);
    }
    void run();
};

#endif