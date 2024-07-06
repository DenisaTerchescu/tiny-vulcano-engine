#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "TinyDevice.hpp"
#include "TinyWindow.hpp"

class TinySwapChain {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

public:
    void init(TinyDevice& device, GLFWwindow* window);
    void cleanup(TinyDevice& device);

    VkSwapchainKHR getSwapChain() const;
    VkFormat getSwapChainImageFormat() const;
    VkExtent2D getSwapChainExtent() const;
    const std::vector<VkImage>& getSwapChainImages() const;
    const std::vector<VkImageView>& getSwapChainImageViews() const;
    VkRenderPass getRenderPass() const;
    const std::vector<VkFramebuffer> getSwapChainFramebuffers() const;

    void createRenderPass(TinyDevice& device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
    void createImageViews(TinyDevice& device);
    VkImageView createImageView(TinyDevice& device, VkImage image, VkFormat format,
        VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    void createFramebuffers(TinyDevice& device);

private:
    VkRenderPass renderPass;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
};

