#define GLM_ENABLE_EXPERIMENTAL
#include "TinyEngine.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/transform.hpp>

void TinyEngine::run() {
    window.initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void TinyEngine::initVulkan() {
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

void TinyEngine::mainLoop() {


	auto stop = std::chrono::high_resolution_clock::now();
	auto previousTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window.getWindow())) {

        glfwPollEvents();
		window.input.updateInput();
        

        {
            double x = 0, y = 0;
            glfwGetCursorPos(window.window, &x, &y);
            window.input.mousePos = { x,y };
        }
		

#pragma region deltaTime

		auto start = std::chrono::high_resolution_clock::now();

		float deltaTime = (std::chrono::duration_cast<std::chrono::nanoseconds>(start - stop)).count() / 1000000000.0;
		stop = std::chrono::high_resolution_clock::now();

		float augmentedDeltaTime = deltaTime;
		if (augmentedDeltaTime > 1.f / 10) { augmentedDeltaTime = 1.f / 10; } //clamp so it doesn't get too big
		if (augmentedDeltaTime < 0) { augmentedDeltaTime = 0; } //in case any wierd thing happens

        calculateFPS(deltaTime);

#pragma endregion

        gameUpdate(deltaTime, window, window.input);

        drawFrame();
    }

    vkDeviceWaitIdle(tinyDevice.getDevice());
}

void TinyEngine::drawFrame() {
    vkWaitForFences(tinyDevice.getDevice(), 1, &tinySync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(tinyDevice.getDevice(), swapChain.getSwapChain(), UINT64_MAX, tinySync.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized) {
        window.framebufferResized = false;
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

    VkSwapchainKHR swapChains[] = { swapChain.getSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(tinyDevice.getPresentationQueue(), &presentInfo);

}

void TinyEngine::cleanup() {

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

void TinyEngine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
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

//void TinyEngine::loadModel() {
//    tinyobj::attrib_t attrib;
//    std::vector<tinyobj::shape_t> shapes;
//    std::vector<tinyobj::material_t> materials;
//    std::string warn, err;
//
//    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
//        throw std::runtime_error(warn + err);
//    }
//
//    std::unordered_map<TinyPipeline::Vertex, uint32_t> uniqueVertices{};
//
//    for (const auto& shape : shapes) {
//        for (const auto& index : shape.mesh.indices) {
//            TinyPipeline::Vertex vertex{};
//            vertex.pos = {
//                attrib.vertices[3 * index.vertex_index + 0],
//                attrib.vertices[3 * index.vertex_index + 1],
//                attrib.vertices[3 * index.vertex_index + 2]
//            };
//
//            vertex.texCoord = {
//                attrib.texcoords[2 * index.texcoord_index + 0],
//                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
//            };
//
//            vertex.color = { 1.0f, 1.0f, 1.0f };
//
//            if (uniqueVertices.count(vertex) == 0) {
//                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
//                vertices.push_back(vertex);
//            }
//
//            indices.push_back(uniqueVertices[vertex]);
//        }
//    }
//}

void TinyEngine::calculateFPS(float deltaTime) {
    frameCount++;
    timeAccumulator += deltaTime;

    if (timeAccumulator >= fpsUpdateInterval) {
        fps = frameCount / timeAccumulator;


        frameCount = 0;
        timeAccumulator = 0.0f;

    }
}

void TinyEngine::lookAround(float deltaTime, float xPos, float yPos) {

    const float constraint = 179.0f;

    yaw += 3 * deltaTime  * xPos;
    pitch += 3 * deltaTime  * yPos; 

    if (yaw > 360) {
        yaw -= 360;
    }

    if (yaw < -360) {
        yaw += 360;
    }


    //if (pitch > constraint) {
    //    pitch = constraint;
    //}
    //else if (pitch < -constraint) {
    //    pitch = -constraint;
    //}

    //glm::vec4 front = {0,0, -1, 1};
   // front = glm::rotate(glm::radians(yaw), glm::vec3{ 0,1,0 }) * front;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
    cameraFront = glm::normalize(front);
   

}
void TinyEngine::gameUpdate(float deltaTime, TinyWindow& window, Input& input)
{
    const float moveSpeed = 0.8f * deltaTime;

    if (input.keyBoard[Button::A].held) {
        pos.x += moveSpeed;
    }

    if (input.keyBoard[Button::D].held) {
        pos.x -= moveSpeed;
    }

    if (input.keyBoard[Button::W].held) {
        pos.z += moveSpeed;
    }

    if (input.keyBoard[Button::S].held) {
        pos.z -= moveSpeed;
    }

    if (input.keyBoard[Button::Q].held) {
        pos.y += moveSpeed;
    }

    if (input.keyBoard[Button::E].held) {
        pos.y -= moveSpeed;
    }

    if (input.rightMouse.held) {

        float xPos = input.delta.x;
        float yPos = input.delta.y;

        std::cout << input.lastPos.x << " " << xPos << '\n';

        lookAround(deltaTime, xPos, yPos);
    }


}
void TinyEngine::updateUniformBuffer(uint32_t currentImage) {

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    TinyBuffer::UniformBufferObject ubo{};
    ubo.model = glm::translate(glm::mat4(1.0f), pos);
    ubo.model = glm::scale(ubo.model, glm::vec3(0.5f, 0.5f, 0.5f));

    glm::vec3 pos(0, 1, -2);

    ubo.view = glm::lookAt({ pos },
        glm::vec3(pos) + cameraFront,
        glm::vec3(0.0f, 1.0f, 0.0f));

    ubo.proj = glm::perspective(glm::radians(45.0f), swapChain.getSwapChainExtent().width / (float)swapChain.getSwapChainExtent().height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    memcpy(tinyBuffer.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

}