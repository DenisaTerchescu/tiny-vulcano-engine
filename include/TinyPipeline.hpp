#pragma once

#include <cstring>
#include <fstream>
#include <glm/glm.hpp>
#include <array>
#include "TinyDevice.hpp"
#include "TinySwapchain.hpp"


class TinyPipeline {
    std::vector<VkDynamicState> states = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
    };

public:
    struct vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(vertex, texCoord);

            return attributeDescriptions;
        }

        bool operator==(const vertex& other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    };


    void init(TinyDevice& device, TinySwapChain& swapChain);
    void cleanup(TinyDevice& device);

    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    void createDescriptorSetLayout(TinyDevice& device);

    VkShaderModule createShaderModule(TinyDevice& device, const std::vector<char>& code);
    static std::vector<char> readFile(const std::string& filename);

    void createGraphicsPipeline(TinyDevice& device, TinySwapChain& swapChain);
};
