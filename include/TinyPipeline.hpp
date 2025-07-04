#pragma once

#include <cstring>
#include <fstream>
#include <glm/glm.hpp>
#include <array>
#include "TinyDevice.hpp"
#include "TinySwapchain.hpp"
#include <iostream>

using namespace std;


class TinyPipeline {
    std::vector<VkDynamicState> states = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
    };

public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
        glm::vec3 normal;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            // normal 
            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3; 
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, normal);

            return attributeDescriptions;
        }

        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && normal == other.normal && texCoord == other.texCoord;
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
