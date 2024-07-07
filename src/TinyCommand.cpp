
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

VkCommandBuffer TinyCommand::beginSingleTimeCommands(TinyDevice& device) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device.getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void TinyCommand::endSingleTimeCommands(VkCommandBuffer commandBuffer, TinyDevice& device) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.getGraphicsQueue());

    vkFreeCommandBuffers(device.getDevice(), commandPool, 1, &commandBuffer);
}

void TinyCommand::createCommandBuffers(TinyDevice& device) {
    commandBuffers.resize(2);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

void TinyCommand::cleanup(TinyDevice& device) {
    vkDestroyCommandPool(device.getDevice(), commandPool, nullptr);
}