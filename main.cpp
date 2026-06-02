#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_runtime.h>
#include <iostream>

const int WIDTH = 1024;
const int HEIGHT = 512;

GLuint texture;             
uchar4* dev_render_buffer;  
GLuint fbo; // Read Framebuffer for easy blitting

extern "C" void launch_kernel(uchar4* d_out, int width, int height, float time);

void initGraphics() {
    // 1. Allocate CUDA Unified Memory
    cudaMallocManaged(&dev_render_buffer, WIDTH * HEIGHT * sizeof(uchar4));

    // 2. Setup OpenGL Texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 3. Create a Framebuffer object to host our texture for direct blitting
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void runSimulation() {
    float time = glfwGetTime();

    // Run your path tracer kernel
    launch_kernel(dev_render_buffer, WIDTH, HEIGHT, time);
    cudaDeviceSynchronize();

    // Upload the fresh CUDA array directly into the OpenGL texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, dev_render_buffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int main() {
    if (!glfwInit()) return -1;

    // We use a COMPATIBILITY profile here because it allows direct blitting 
    // to the backbuffer without writing complex custom vertex and fragment shaders.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CUDA Path Tracer Pipeline", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable V-Sync (Locks frame rate to your monitor)
    glewInit();

    initGraphics();

    while (!glfwWindowShouldClose(window)) {
        // Compute the next frame on your RTX 5080
        runSimulation();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // --- THE PRESENTATION BRIDGE ---
        // Bind our texture-carrying framebuffer as the source, and the window screen as the destination
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 

        // Copy the texture directly to the screen pixels instantly via hardware blitting
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 
                          0, 0, WIDTH, HEIGHT, 
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        // -------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cudaFree(dev_render_buffer);
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &texture);
    glfwDestroyWindow(window);
    glfwTerminate();
    
    cudaDeviceReset();
    return 0;
}