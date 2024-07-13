
#include "TinyEngine.hpp"

int main() {

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








