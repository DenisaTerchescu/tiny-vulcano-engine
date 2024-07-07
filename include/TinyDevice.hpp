#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

class TinyDevice {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentationQueue;

        void init(const VkInstance& instance, GLFWwindow* window);
        VkSurfaceKHR getSurface() const;
        VkDevice getDevice() const;
        VkPhysicalDevice getPhysicalDevice() const;
        VkQueue getGraphicsQueue() const;
        VkQueue getPresentationQueue() const;
       
        void createSurface(VkInstance instance, GLFWwindow* window);
        void pickPhysicalDevice(VkInstance vkInstance);
        void createLogicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        TinyDevice::QueueFamilyIndices findQueueFamilies();
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void cleanUp(const VkInstance& instance);
};