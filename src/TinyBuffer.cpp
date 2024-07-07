#include "TinyBuffer.hpp"

void TinyBuffer::init(TinyDevice& device) {
    // Placeholder for potential initialization logic
}

void TinyBuffer::cleanup(TinyDevice& device) {
    vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
    vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);
    vkDestroyBuffer(device.getDevice(), indexBuffer, nullptr);
    vkFreeMemory(device.getDevice(), indexBufferMemory, nullptr);
}

void TinyBuffer::createVertexBuffer(TinyDevice& device, TinyCommand& command, const std::vector<Vertex>& vertices) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device.getDevice(), stagingBufferMemory);

    createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    copyBuffer(device, command, stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}

void TinyBuffer::createIndexBuffer(TinyDevice& device, TinyCommand& command, const std::vector<uint32_t>& indices) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(device.getDevice(), stagingBufferMemory);

    createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(device, command, stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}

void TinyBuffer::createBuffer(TinyDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
  // allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device.getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device.getDevice(), buffer, bufferMemory, 0);
}

void TinyBuffer::copyBuffer(TinyDevice& device, TinyCommand& command, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    //VkCommandBuffer commandBuffer = command.beginSingleTimeCommands();

    //VkBufferCopy copyRegion{};
    //copyRegion.srcOffset = 0;
    //copyRegion.dstOffset = 0;
    //copyRegion.size = size;
    //vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

   // device.endSingleTimeCommands(commandBuffer);
}