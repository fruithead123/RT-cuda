#include <pathtracer/Frontend.h>
#include <iostream>
#include <fstream>
#include <filesystem>

/* Window Frontend Implementation*/

bool WindowFrontend::init(int width, int height) {
    w = width;
    h = height;

    if (!glfwInit()){
        std::cerr << "Could not initialize GLFW!" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwCreateWindow(w, h, "CUDA Path Tracer", NULL, NULL);
    if (!window){
        std::cerr << "Could not create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Vsync
    
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW Initialization Failed!" << std::endl;
        return false;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);
    return true;
}

void WindowFrontend::render(const uchar4* dev_render_buffer) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Upload the CUDA buffer to the texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, dev_render_buffer);

    // Draw screen quad
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
    glEnd();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool WindowFrontend::shouldClose() {
    return glfwWindowShouldClose(window);
}

WindowFrontend::~WindowFrontend() {
    if (texture) glDeleteTextures(1, &texture);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}


/* Image Frontend Implementation */

bool ImageFrontend::init(int width, int height){
    w = width;
    h = height;

    if (!std::filesystem::exists(output_root)) {
        std::filesystem::create_directories(output_root);
        std::cout << "Created output directory: " << std::filesystem::absolute(output_root) << std::endl;
    }

    return true;
}

void ImageFrontend::render(const uchar4* dev_render_buffer){
    // Construct a unique filename for this frame
    std::string filename = "render" + std::to_string(currentFrame++) + ".ppm";
    std::filesystem::path fullPath = output_root / filename;
    std::cout << "Saving to " << filename << std::endl;
    
    // Open file in binary mode
    std::ofstream file(fullPath, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    // 1. Write the PPM Header
    // P6 means binary RGB, followed by width, height, and max color value (255)
    file << "P6\n" << w << " " << h << "\n255\n";

    // 2. Write the Pixel Data
    // Your buffer is uchar4 (RGBA), but PPM expects strict 24-bit RGB (3 bytes per pixel).
    // We loop through the managed buffer and drop the Alpha channel on the fly.
    for (int i = 0; i < w * h; ++i) {
        uchar4 pixel = dev_render_buffer[i];
        
        // Write Red, Green, and Blue bytes sequentially
        file.put(pixel.x); // R
        file.put(pixel.y); // G
        file.put(pixel.z); // B
    }

    file.close();
    std::cout << "Successfully saved snapshot to: " << fullPath << std::endl;
}

bool ImageFrontend::shouldClose() {
    return currentFrame >= maxFrames;
}

void ImageFrontend::setMaxFrames(int maxFrames){
    this->maxFrames = maxFrames;
}