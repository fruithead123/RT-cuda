#include <pathtracer/Engine.h>
#include <pathtracer/Scene.cuh>
#include <memory>

const int WIDTH = 1280;
const int HEIGHT = 720;

int main(){
    auto scene = std::make_unique<Scene>();
    scene->addSphere(Vec3(0.0f, 0.0f, 10.0f), 1.0f);
    scene->hCamera.setup(
        Vec3(0.0, 0.0, 0.0),
        Vec3(0.0, 0.0, 1.0),
        Vec3(0.0, 1.0, 0.0),
        90.0, 
        (float)WIDTH / HEIGHT
    );
    scene->hCamera.dumpState();

    // Window
    std::unique_ptr<WindowFrontend> w_frontend = std::make_unique<WindowFrontend>();

    // Headless
    std::unique_ptr<ImageFrontend> h_frontend = std::make_unique<ImageFrontend>();
    h_frontend->setMaxFrames(1);

    Engine engine(std::move(w_frontend));
    engine.setScreenSize(WIDTH, HEIGHT);
    engine.setScene(std::move(scene));
    engine.run();

}