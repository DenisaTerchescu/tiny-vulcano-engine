
#include <GLFW/glfw3.h>
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3native.h>


#include <cstdlib>
#include <vector>
#include <cstring>
#include <set>

#include <glm/glm.hpp>
#include <array>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "TinyWindow.hpp"
#include "TinyInstance.hpp"
#include "TinyDevice.hpp"
#include "TinySwapChain.hpp"
#include "TinyPipeline.hpp"
#include "TinyCommand.hpp"
#include "TinyBuffer.hpp"
#include "TinyTexture.hpp"
#include "TinyDepth.hpp"
#include "TinySync.hpp"

const std::string MODEL_PATH = "C:/Users/Denisa/Desktop/TinyVulcanoEngine/resources/models/room.obj";

const int MAX_FRAMES_IN_FLIGHT = 2;

namespace std {
    template<> struct hash<TinyPipeline::Vertex> {
        size_t operator()(TinyPipeline::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

std::vector<TinyPipeline::Vertex> vertices = {
    
    {{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    
    {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0f}, {0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}}
};

std::vector<uint16_t> indices = {
    
    0, 1, 2,
    2, 3, 0,

    1, 5, 6,
    6, 2, 1,
    
    7, 6, 5,
    5, 4, 7,
    
    4, 0, 3,
    3, 7, 4,
    
    4, 5, 1,
    1, 0, 4,
   
    3, 2, 6,
    6, 7, 3
};

class TinyVulcanoEngine {

public:
    void run() {
        window.initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    TinyWindow window;
    TinyInstance instance;
    TinyDevice tinyDevice;
    TinySwapChain swapChain;
    TinyPipeline pipeline;
    TinyCommand command;

    TinySync tinySync;

    TinyBuffer tinyBuffer;

    TinyDepth depth;

    TinyTexture texture;

    //std::vector<Vertex> vertices;
    //std::vector<uint32_t> indices;

    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    void initVulkan() {
       instance.init();
       tinyDevice.init(instance.getInstance(), window.getWindow());
       swapChain.init(tinyDevice, window.getWindow());
       pipeline.init(tinyDevice, swapChain);
       swapChain.createFramebuffers(tinyDevice);
       command.createCommandPool(tinyDevice);
       depth.createDepthResources(tinyDevice, swapChain, texture);
       texture.init(tinyDevice, command, tinyBuffer);

       //loadModel();
       tinyBuffer.createVertexBuffer(tinyDevice, command, vertices);
       tinyBuffer.createIndexBuffer(tinyDevice, command, indices);
       tinyBuffer.createUniformBuffers(tinyDevice, pipeline, texture.textureImageView, texture.textureSampler);

       command.createCommandBuffers(tinyDevice);
       tinySync.createSyncObjects(tinyDevice);
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window.getWindow())) {
            glfwPollEvents();
            drawFrame();
        }
        
        vkDeviceWaitIdle(tinyDevice.getDevice());
    }


    void drawFrame() {
        vkWaitForFences(tinyDevice.getDevice(), 1, &tinySync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(tinyDevice.getDevice(), swapChain.getSwapChain(), UINT64_MAX, tinySync.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            swapChain.recreateSwapChain(tinyDevice, window);
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
       
        vkResetFences(tinyDevice.getDevice(), 1, &tinySync.inFlightFences[currentFrame]);

        vkResetCommandBuffer(command.commandBuffers[currentFrame], 0);

        recordCommandBuffer(command.commandBuffers[currentFrame], imageIndex);

        updateUniformBuffer(currentFrame);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { tinySync.imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command.commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = { tinySync.renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(tinyDevice.getGraphicsQueue(), 1, &submitInfo, tinySync.inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapChain.getSwapChain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        presentInfo.pResults = nullptr; // Optional
        vkQueuePresentKHR(tinyDevice.getPresentationQueue(), &presentInfo);

    }
#pragma region CleanUp

    void cleanup() {

        swapChain.cleanup(tinyDevice);

        texture.cleanup(tinyDevice);

        tinyBuffer.cleanupUniformBuffers(tinyDevice);

        pipeline.cleanup(tinyDevice);

        tinySync.cleanup(tinyDevice);

        command.cleanup(tinyDevice);

        tinyBuffer.cleanup(tinyDevice);

        tinyDevice.cleanUp(instance.getInstance());

        instance.cleanUp();

        window.cleanUp();

        glfwTerminate();
    }
#pragma endregion CleanUp

#pragma region Command_Buffers

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain.getRenderPass();
        renderPassInfo.framebuffer = swapChain.getSwapChainFramebuffers()[imageIndex];

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

        VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain.getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain.getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChain.getSwapChainExtent();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipeline);

        VkBuffer vertexBuffers[] = { tinyBuffer.vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

       vkCmdBindIndexBuffer(commandBuffer, tinyBuffer.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

       // for loading models
        //vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &tinyBuffer.descriptorSets[currentFrame], 0, nullptr);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        vkCmdEndRenderPass(commandBuffer);


        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

#pragma endregion Command_Buffers


#pragma region Load_model

   void loadModel() {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<TinyPipeline::Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                TinyPipeline::Vertex vertex{};
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
#pragma endregion Load_model 

    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
        TinyBuffer::UniformBufferObject ubo{};
         ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
       
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        ubo.proj = glm::perspective(glm::radians(45.0f), swapChain.getSwapChainExtent().width / (float)swapChain.getSwapChainExtent().height, 0.1f, 10.0f);

        ubo.proj[1][1] *= -1;

        memcpy(tinyBuffer.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    
    }

};

int main() {

    TinyVulcanoEngine engine;

    try {
        engine.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}








