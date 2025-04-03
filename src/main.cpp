
#include "TinyEngine.hpp"

int main() {

    ShellExecute(0, "open", (RESOURCES_PATH "shaders/compile_simple.bat"), nullptr, nullptr, SW_SHOW);

    TinyEngine engine;

    try {
        engine.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}








