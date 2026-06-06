#include <pathtracer/Engine.h>
#include <iostream>

#define CUDA_CHECK(ans) { cudaAssert((ans), __FILE__, __LINE__); }
inline void cudaAssert(cudaError_t code, const char *file, int line, bool abort=true) {
   if (code != cudaSuccess) {
      std::cerr << "CUDA Error: " << cudaGetErrorString(code) << " " << file << " " << line << std::endl;
      if (abort) exit(code);
   }
}

Engine::Engine() 
    : window(nullptr), dev_render_buffer(nullptr), texture(0) {}

Engine::~Engine() {
    cleanup();
}

void Engine::initGraphics() {
    // 1. Allocate CUDA Unified Memory & Prefetch
    CUDA_CHECK(cudaMallocManaged(&dev_render_buffer, WIDTH * HEIGHT * sizeof(uchar4)));
    int device = 0;
    cudaGetDevice(&device);
    cudaMemPrefetchAsync(dev_render_buffer, WIDTH * HEIGHT * sizeof(uchar4), device, nullptr);

    // 2. Setup OpenGL Texture Asset
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Use GL_NEAREST for crisp, pixel-perfect path tracer outputs
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Allocate the texture memory on the OpenGL driver side
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Engine::runSim() {
    float time = static_cast<float>(glfwGetTime());

    // Run your path tracer kernel directly into managed memory
    launch_kernel(dev_render_buffer, WIDTH, HEIGHT, time);
    CUDA_CHECK(cudaDeviceSynchronize());

    // Safe, driver-friendly texture update loop
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, dev_render_buffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Engine::cleanup() {
    if (dev_render_buffer) {
        cudaFree(dev_render_buffer);
        dev_render_buffer = nullptr;
    }
    if (texture) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
    cudaDeviceReset();
}

int Engine::run() {
    if (!glfwInit()) {
        std::cerr << "GLFW Init failed!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "CUDA Path Tracer Pipeline", NULL, NULL);
    if (!window) {
        std::cerr << "Could not create window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync
    
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW Initialization Failed!" << std::endl;
        return -1;
    }

    initGraphics();

    // Enable standard 2D texturing hardware texturing pass
    glEnable(GL_TEXTURE_2D);

    while (!glfwWindowShouldClose(window)) {
        // Run path tracer & upload data to our texture
        runSim();

        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the texture to a full screen quad using fixed-function compatibility mode.
        // This drops the requirement for manual FBO blitting logic.
        glBindTexture(GL_TEXTURE_2D, texture);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);

        // (Future ImGui::Render() calls can safely drop right here!)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}