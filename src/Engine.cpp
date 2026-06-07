#include <pathtracer/Engine.h>

void Engine::init(){
    cudaMallocManaged(&dev_render_buffer, WIDTH * HEIGHT * sizeof(uchar4));
    output->init(WIDTH, HEIGHT);
}

void Engine::run() {
    init();
    while (!output->shouldClose()) {
        float time = static_cast<float>(glfwGetTime());
        // 1. Run simulation step
        // 2. Run path tracer kernel directly into dev_render_buffer
        launch_kernel(dev_render_buffer, WIDTH, HEIGHT, time);
        cudaDeviceSynchronize();

        // 3. Delegate presentation entirely to the frontend choice
        output->render(dev_render_buffer);
    }
}