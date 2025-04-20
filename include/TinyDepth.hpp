#pragma once


#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
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

    void createDepthResources(TinyDevice& device, TinyCommand& command, TinySwapChain& swapChain, TinyTexture& tinyTexture);
    VkFormat findSupportedFormat(TinyDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    
    VkFormat findDepthFormat(TinyDevice& device);

};