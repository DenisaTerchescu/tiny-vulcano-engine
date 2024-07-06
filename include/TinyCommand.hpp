#pragma once

#include <vulkan/vulkan.h>
#include "TinyDevice.hpp"
#include "TinySwapchain.hpp"
#include "TinyPipeline.hpp"

class TinyCommand {
public:
    void init(TinyDevice& device, TinySwapChain& swapChain, TinyPipeline& pipeline);
    void cleanup(TinyDevice& device);

    VkCommandBuffer beginSingleTimeCommands(TinyDevice& device);
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, TinyDevice& device);

    const std::vector<VkCommandBuffer>& getCommandBuffers() const;

private:
    void createCommandPool(TinyDevice& device);
    void createCommandBuffers(TinyDevice& device, TinySwapChain& swapChain, TinyPipeline& pipeline);

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
