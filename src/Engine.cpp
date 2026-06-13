#include <pathtracer/Engine.h>
#include <iostream>

void Engine::init(){
    cudaMallocManaged(&dev_render_buffer, WIDTH * HEIGHT * sizeof(uchar4));
    output->init(WIDTH, HEIGHT);

    cudaEventCreate(&cudaStartTime);
    cudaEventCreate(&cudaEndTime);
}

Engine::~Engine(){
    if (scene) scene->freeDevice();
    cudaFree(dev_render_buffer);

    cudaEventDestroy(cudaStartTime);
    cudaEventDestroy(cudaEndTime);
}

void Engine::run() {
    init();
    if (!scene){
        std::cout << "Scene is null!" << std::endl;
        return;
    }
    scene->uploadToDevice();

    // Profiling vars
    float lastKernelTimeMs = 0.0f;

    while (!output->shouldClose()) {
        float time = static_cast<float>(glfwGetTime());
        cudaEventRecord(cudaStartTime, 0);

        DeviceScene dScene = scene->getDeviceScene();
        launch_kernel(dev_render_buffer, WIDTH, HEIGHT, time, dScene);
        cudaEventRecord(cudaEndTime, 0);
        cudaDeviceSynchronize();

        cudaEventElapsedTime(&lastKernelTimeMs, cudaStartTime, cudaEndTime);

        // 3. Delegate presentation entirely to the frontend choice
        output->render(dev_render_buffer, lastKernelTimeMs);
    }
}