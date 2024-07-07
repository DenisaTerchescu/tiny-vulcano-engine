#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>
#include <array>
#include "TinyDevice.hpp"
#include "TinyCommand.hpp"

class TinyBuffer {
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
    };

    void cleanup(TinyDevice& device);

    void createVertexBuffer(TinyDevice& device, TinyCommand& command, const std::vector<TinyPipeline::vertex>& vertices);
    void createIndexBuffer(TinyDevice& device, TinyCommand& command, const std::vector<uint16_t>& indices);

    void createBuffer(TinyDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(TinyDevice& device, TinyCommand& command, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};


