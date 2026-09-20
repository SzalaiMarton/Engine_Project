#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <exception>

constexpr int REQUIREMENT_NOT_MET = -1;
constexpr int OPTIONAL_NOT_MET = 0;
constexpr int REQUIREMENT_MET = 1000;
constexpr int OPTIONAL_MET = 100;

struct RequiredGPUFeatures {
    static inline constexpr bool hasGeometryShaders(const vk::PhysicalDeviceFeatures&  features) {
        return features.geometryShader;
    }

    static inline constexpr int runAll(const vk::PhysicalDeviceFeatures&  features) {
        int score = 0;
        
        if (!hasGeometryShaders(features)) {
            throw std::exception();
        }
        
        return score;
    }
};

struct RequiredGPUProperties {
    static inline constexpr int isDiscrete(const vk::PhysicalDeviceProperties&  properties) {
        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            return REQUIREMENT_MET;
        return REQUIREMENT_NOT_MET;
    }

    static inline constexpr int runAll(const vk::PhysicalDeviceProperties&  properties) {
        int score = 0;

        if (auto t_score = isDiscrete(properties) != REQUIREMENT_NOT_MET) {
            score += t_score;
        } else {
            throw std::exception();
        }

        score += properties.limits.maxImageDimension2D;

        return score;
    }
};

struct OptionalGPUFeatures {
    static inline constexpr int runAll(const vk::PhysicalDeviceFeatures&  features) {
        return 0;
    }
};

struct OptionalGPUProperties {
    static inline constexpr int runAll(const vk::PhysicalDeviceProperties&  properties) {
        return 0;
    }
};