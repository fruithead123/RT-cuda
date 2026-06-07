#include <pathtracer/Engine.h>
#include <memory>


int main(){
    // Scene scene = ...

    // Window
    std::unique_ptr<WindowFrontend> w_frontend = std::make_unique<WindowFrontend>();

    // Headless
    std::unique_ptr<ImageFrontend> h_frontend = std::make_unique<ImageFrontend>();
    h_frontend->setMaxFrames(1);

    Engine engine(std::move(h_frontend));
    engine.run();

}