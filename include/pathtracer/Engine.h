#ifndef ENGINE_H
#define ENGINE_H

#include <pathtracer/Frontend.h>
#include <memory>

extern "C" void launch_kernel(uchar4* d_out, int width, int height, float time);

class Engine {
private:
    std::unique_ptr<Frontend> output;
    uchar4* dev_render_buffer;
    int WIDTH = 1280;
    int HEIGHT = 720;

    void init();

public:
    Engine(std::unique_ptr<Frontend> display) : output(std::move(display)) {}
    ~Engine(){
        cudaFree(dev_render_buffer);
    }
    
    void run();
};

#endif