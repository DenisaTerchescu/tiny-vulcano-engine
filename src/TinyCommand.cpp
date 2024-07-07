
#include "TinyCommand.hpp"

void TinyCommand::createCommandPool(TinyDevice& device) {
    TinyDevice::QueueFamilyIndices queueFamilyIndices = device.findQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(device.getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void TinyCommand::cleanup(TinyDevice& device) {
    vkDestroyCommandPool(device.getDevice(), commandPool, nullptr);
}