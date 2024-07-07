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
    };

    void cleanup(TinyDevice& device);
    void cleanupUniformBuffers(TinyDevice& device);

    void createVertexBuffer(TinyDevice& device, TinyCommand& command, const std::vector<TinyPipeline::Vertex>& vertices);
    void createIndexBuffer(TinyDevice& device, TinyCommand& command, const std::vector<uint16_t>& indices);

    void createBuffer(TinyDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(TinyDevice& device, TinyCommand& command, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void createUniformBuffers(TinyDevice& device, TinyPipeline pipeline, VkImageView textureImageView,
        VkSampler textureSampler);
    void createDescriptorPool(TinyDevice& device);
    void createDescriptorSets(TinyDevice& device, TinyPipeline pipeline, VkImageView textureImageView,
        VkSampler textureSampler);

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    void updateUniformBuffer(TinySwapChain& swapChain, uint32_t currentImage);

    const int MAX_FRAMES_IN_FLIGHT = 2;
};


