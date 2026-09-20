#include "triangle.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main() {
    try {
        TestingEnvironment env;
        env.run();
    } catch (const vk::SystemError& err) {
        std::cerr << "Vulkan error: " << err.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
