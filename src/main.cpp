
//#include "App.h"
#include "core/Application.h"
#include <iostream>

int main() {
    try {
        ApplicationConfig config;
        config.windowProps.title = "Galaxy Renderer";
        config.windowProps.width = 1920;
        config.windowProps.height = 1080;
        config.windowProps.isResizable = true;

        Application app(config);
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}