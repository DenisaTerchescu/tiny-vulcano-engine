#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include "TinyDevice.hpp"
#include "TinySwapchain.hpp"
#include "TinyBuffer.hpp"
#include "TinyCommand.hpp"


class TinyTexture{
public:
    void init(TinyDevice& device, TinyCommand& command, TinyBuffer& tinyBuffer);
    void cleanup(TinyDevice& device);

    uint32_t mipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    VkImageView textureImageView;
    VkSampler textureSampler;

    const std::string TEXTURE_PATH = "C:/Users/Denisa/Desktop/TinyVulcanoEngine/resources/textures/glass.png";

    void createTextureImage(TinyDevice& device, TinyCommand& command, TinyBuffer& tinyBuffer);
    void copyBuffer(TinyDevice& device, TinyCommand& command, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void transitionImageLayout(TinyDevice& device, TinyCommand& command, VkImage image, VkFormat format, VkImageLayout oldLayout,
        VkImageLayout newLayout, uint32_t mipLevels);

    void copyBufferToImage(TinyDevice& device, TinyCommand& command, VkBuffer buffer,
        VkImage image, uint32_t width, uint32_t height);

    void createImage(TinyDevice& device, uint32_t width, uint32_t height, uint32_t mipLevels,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void createTextureImageView(TinyDevice& device);
    VkImageView createImageView(TinyDevice& device, VkImage image, VkFormat format,
        VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    void createTextureSampler(TinyDevice& device);

    void generateMipmaps(TinyDevice& device, TinyCommand& command,
        VkImage image, VkFormat imageFormat, int32_t texWidth,
        int32_t texHeight, uint32_t mipLevels);
};


