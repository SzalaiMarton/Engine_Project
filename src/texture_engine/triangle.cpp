#include "triangle.h"
#include <algorithm>
#include <cstring>
#include <ranges>
#include <iostream>

void TestingEnvironment::mainloop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        for (auto& obj : this->objects) {
            obj->render();
        }
    }
}

void TestingEnvironment::initWindow() {
    // initialize glfw
    glfwInit();
    // tells glfw not to use openGL -> it was designed to use openGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // disable window resize for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    this->window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void TestingEnvironment::run() {
    this->initWindow();
    this->initVulkan();
    this->add(new Triangle());
    this->mainloop();
    this->cleanup();
}

void TestingEnvironment::add(Triangle* triangle) {
    this->objects.push_back(triangle);
}

void TestingEnvironment::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void TestingEnvironment::initVulkan() {
    VkInstance instance;
    this->appInfo = VkApplicationInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "First Triangle",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };

    // Get the required instance extensions from GLFW.
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Check if the required GLFW extensions are supported by the Vulkan implementation.
    auto extensionProperties = this->context.enumerateInstanceExtensionProperties();
    for (uint32_t i = 0; i < glfwExtensionCount; i++) {
        if (
            std::ranges::none_of(
                extensionProperties, 
                [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
                { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }
        )) 
        {
            std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
        }
    }

    this->createInfo = VkInstanceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions,
    };

    this->instance = vk::raii::Instance(context, createInfo);
    this->physicalDevice = this->instance.enumeratePhysicalDevices().front();
    this->device = vk::raii::Device(this->physicalDevice, vk::DeviceCreateInfo());

    this->buffer = vk::raii::Buffer(this->device, vk::BufferCreateInfo());
}

void Triangle::render() {
    std::cout << "[LOG] Rendering object: " << this << std::endl;
}
