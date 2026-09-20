#include "triangle.h"
#include <algorithm>
#include <cstring>
#include <ranges>
#include <iostream>
#include <cmath>

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

void TestingEnvironment::pickPhysicalDevice() {
    if (this->instance->enumeratePhysicalDevices().empty()) {
        throw std::runtime_error("[ERROR] Failed to find GPUs with Vulkan support!");
    }

    auto physicalDevices = vk::raii::PhysicalDevices(*this->instance);

    for (auto pd: physicalDevices) {
        try {
            this->evalDevice(pd);
        } catch (std::exception& e) {
            continue;
        }
    }

    if (!this->physicalDeviceCandidates.empty() && this->physicalDeviceCandidates.rbegin()->first > 0) {
        this->physicalDevice = this->physicalDeviceCandidates.rbegin()->second;
    } else {
        std::runtime_error("[ERROR] Failed to find a suitable GPU!");
    }

    // this->physicalDevice.emplace(this->instance->enumeratePhysicalDevices().front());
}

void TestingEnvironment::evalDevice(const vk::raii::PhysicalDevice& physicalDevice) {
    auto properties = physicalDevice.getProperties();
    auto features = physicalDevice.getFeatures();
    int score = 0;

    try {
        score += RequiredGPUFeatures::runAll(features);
        score += RequiredGPUProperties::runAll(properties);
        score += OptionalGPUFeatures::runAll(features);
        score += OptionalGPUProperties::runAll(properties);

        this->physicalDeviceCandidates.insert(std::make_pair(score, physicalDevice));
    } catch (std::exception& e) {
        LOG("[ERROR] " << &physicalDevice << "faile during requirement checks!");
    }
}

vk::DeviceCreateInfo TestingEnvironment::getDeviceInfo()
{
    auto queueFamilies = this->physicalDevice->getQueueFamilyProperties();

    uint32_t graphicsFamily = ~0u;
    for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            graphicsFamily = i;
            break;
        }
    }
    if (graphicsFamily == ~0u) {
        throw std::runtime_error("[ERROR] No graphics queue family found");
    }
    this->graphicsQueueFamily = graphicsFamily;

    float priority = 1.0f;
    vk::DeviceQueueCreateInfo queueInfo{};
    queueInfo.queueFamilyIndex = graphicsFamily;
    queueInfo.queueCount        = 1;
    queueInfo.pQueuePriorities  = &priority;

    vk::DeviceCreateInfo deviceInfo{};
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos    = &queueInfo;

    return deviceInfo;
}

std::pair<uint32_t, const char **> TestingEnvironment::getExtensionInfo() {
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
            std::runtime_error("[ERROR] Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
        }
    }

    return std::pair<uint32_t, const char **>({glfwExtensionCount, glfwExtensions});
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

    auto extensionInfo = this->getExtensionInfo();

    this->createInfo = VkInstanceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = extensionInfo.first,
        .ppEnabledExtensionNames = extensionInfo.second,
    };

    this->instance.emplace(this->context, createInfo);
    this->pickPhysicalDevice();
    this->device.emplace(*this->physicalDevice, this->getDeviceInfo());
    this->buffer.emplace(*this->device, vk::BufferCreateInfo());
}

void Triangle::render() {
    std::cout << "[LOG] Rendering object: " << this << std::endl;
}
