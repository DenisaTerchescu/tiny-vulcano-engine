#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>
#include <array>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include "TinyDevice.hpp"
#include "TinyCommand.hpp"

class TinyBuffer {
public:
    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec3 viewPos;
    };

    void cleanup(TinyDevice& device);
    void cleanupUniformBuffers(TinyDevice& device);

    void createVertexBuffer(TinyDevice& device, TinyCommand& command, const std::vector<TinyPipeline::Vertex>& vertices,
        VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);
    void createIndexBuffer(TinyDevice& device, TinyCommand& command, 
        const std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory);

    void createBuffer(TinyDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(TinyDevice& device, TinyCommand& command, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void createUniformBuffers(TinyDevice& device, TinyPipeline pipeline, 
        const std::vector<VkImageView>& textureImageViews, const std::vector<VkSampler>& textureSamplers);
    void createDescriptorPool(TinyDevice& device);
    void createDescriptorSets(TinyDevice& device, TinyPipeline pipeline, 
        const std::vector<VkImageView>& textureImageViews, const std::vector<VkSampler>& textureSamplers);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkBuffer planeVertexBuffer;
    VkDeviceMemory planeVertexBufferMemory;
    VkBuffer planeIndexBuffer;
    VkDeviceMemory planeIndexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;         // [frame]
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;


    std::vector<VkDescriptorSet> descriptorSets; // [object][frame]

    VkDescriptorPool descriptorPool;

    const int MAX_FRAMES_IN_FLIGHT = 2;
    size_t objectCount = 4095;
};


