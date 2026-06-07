#ifndef FRONTEND_H
#define FRONTEND_H

#include <cuda_runtime.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <filesystem>

class Frontend{
public:
    virtual ~Frontend() = default;

    virtual bool init(int width, int height) = 0;
    virtual void render(const uchar4* dev_render_buffer) = 0;
    virtual bool shouldClose() = 0;

};


class WindowFrontend : public Frontend {
public:
    ~WindowFrontend();
    bool init(int width, int height);
    void render(const uchar4* dev_render_buffer);
    bool shouldClose();

private:
    GLFWwindow* window = nullptr;
    GLuint texture = 0;
    int w, h;
};

class ImageFrontend : public Frontend {
public:
    bool init(int width, int height);
    void render(const uchar4* dev_render_buffer);
    bool shouldClose();
    void setMaxFrames(int maxFrames);

private:
    int w, h;
    int currentFrame = 0;
    int maxFrames = 0;
    std::filesystem::path output_root = "./output";
};

#endif