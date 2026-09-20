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
#include <map>
#include <array>
#include "GpuRequirements.h"

class Triangle;
struct RequiredGPUFeatures;
struct OptionalGPUFeatures;
struct RequiredGPUProperties;
struct OptionalGPUProperties;


constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

class TestingEnvironment {
public:
    TestingEnvironment() = default;

    void run();
    void add(Triangle* triangle);

private:
    std::vector<Triangle*> objects{};
    VkApplicationInfo appInfo;
    vk::InstanceCreateInfo createInfo;
    GLFWwindow* window = nullptr;
    vk::raii::Context context;
    std::optional<vk::raii::Instance> instance;
    std::optional<vk::raii::Device> device;
    std::optional<vk::raii::PhysicalDevice> physicalDevice;
    std::optional<vk::raii::Buffer> buffer;

    uint32_t graphicsQueueFamily = 0;
    std::multimap<int, vk::raii::PhysicalDevice> physicalDeviceCandidates;

    void initWindow();
    void initVulkan();
    void mainloop();
    void cleanup();

    void pickPhysicalDevice();
    void evalDevice(const vk::raii::PhysicalDevice& physicalDevice);
    vk::DeviceCreateInfo getDeviceInfo();
    std::pair<uint32_t, const char**> getExtensionInfo();
};

class Triangle {
public:
    void render();
private:

};
