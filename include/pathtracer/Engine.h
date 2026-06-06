#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_runtime.h>
#include <iostream>

extern "C" void launch_kernel(uchar4* d_out, int width, int height, float time);

class Engine{
public:
    Engine() {};
    int run();

private:
    void initGraphics();
    void runSim();

private:
    // Texture and frame buffer for rendering
    GLuint texture;
    uchar4* dev_render_buffer;
    GLuint fbo;

    // Window
    const int WIDTH{1024};
    const int HEIGHT{512};
    GLFWwindow* window = nullptr;
};