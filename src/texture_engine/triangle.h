#pragma once
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

class Triangle;

class TestingEnvironment {
public:
    TestingEnvironment() = default;

    void run();
    void add(Triangle* triangle);

private:
    std::vector<Triangle*>          objects{};
    VkApplicationInfo               appInfo;
    vk::InstanceCreateInfo          createInfo;
    GLFWwindow*                     window = nullptr;
    vk::raii::Context               context;
    vk::raii::Instance              instance = nullptr;
    vk::raii::Device                device;
    vk::raii::PhysicalDevice        physicalDevice;
    vk::raii::Buffer                buffer;

    void initWindow();
    void initVulkan();
    void mainloop();
    void cleanup();
};

class Triangle {
public:
    void render();
private:

};
