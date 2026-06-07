#include <pathtracer/Engine.h>
#include <memory>


int main(){
    // Scene scene = ...

    WindowFrontend frontend;
    std::unique_ptr<WindowFrontend> p_frontend = std::make_unique<WindowFrontend>(frontend);

    Engine engine(std::move(p_frontend));
    engine.run();

}