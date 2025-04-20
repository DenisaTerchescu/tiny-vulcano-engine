
#include "TinyDepth.hpp"

void TinyDepth::createDepthResources(TinyDevice& device, TinyCommand& command, TinySwapChain& swapChain, TinyTexture& tinyTexture) {
    VkFormat depthFormat = findDepthFormat(device);

    tinyTexture.createImage(device, swapChain.getSwapChainExtent().width, swapChain.getSwapChainExtent().height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
    depthImageView = tinyTexture.createImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    tinyTexture.transitionImageLayout(device, command, depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

}

VkFormat TinyDepth::findSupportedFormat(TinyDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device.getPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
        else {
            cout << "NO FORMAT!!";
        }
    }

    throw std::runtime_error("Failed to find supported format!");
}

VkFormat TinyDepth::findDepthFormat(TinyDevice& device) {
    return findSupportedFormat(device,
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}