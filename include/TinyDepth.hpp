#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include "TinyDevice.hpp"
#include "TinySwapchain.hpp"
#include "TinyTexture.hpp"

class TinyDepth {
public:
    void init();
    void cleanup();

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    void createDepthResources(TinyDevice& device, TinySwapChain& swapChain, TinyTexture& texture);
    VkFormat findSupportedFormat(TinyDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    
    VkFormat findDepthFormat(TinyDevice& device);

};