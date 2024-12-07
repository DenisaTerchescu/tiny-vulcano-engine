#define GLM_ENABLE_EXPERIMENTAL
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "TinyEngine.hpp"
#include "TinyMathLibrary.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

void TinyEngine::run() {
    window.initWindow();
    initVulkan();
    initImgui();
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
        window.input.updateTinyInput();

        {
            double x = 0, y = 0;
            glfwGetCursorPos(window.window, &x, &y);
            window.input.mousePos = { x,y };
        }
       

        auto start = std::chrono::high_resolution_clock::now();

        float deltaTime = (std::chrono::duration_cast<std::chrono::nanoseconds>(start - stop)).count() / 1000000000.0;
        stop = std::chrono::high_resolution_clock::now();

        float optimizedDeltaTime = deltaTime;
        if (optimizedDeltaTime > 1.f / 10) { optimizedDeltaTime = 1.f / 10; } // bugfix
        if (optimizedDeltaTime < 0) { optimizedDeltaTime = 0; } 

        drawUI();

        calculateFPS(deltaTime);

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
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetFences(tinyDevice.getDevice(), 1, &tinySync.inFlightFences[currentFrame]);

    vkResetCommandBuffer(command.commandBuffers[currentFrame], 0);

    recordCommandBuffer(command.commandBuffers[currentFrame], imageIndex);
  
    //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command.commandBuffers[currentFrame]);

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

    presentInfo.pResults = nullptr;
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

    VkBuffer vertexBuffers[] = { tinyBuffer.vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, tinyBuffer.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
  
    // Draw first cube
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &tinyBuffer.descriptorSetsCube1[currentFrame], 0, nullptr);
    glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(glassContainer.x, glassContainer.y, glassContainer.z));
    cubeModel = glm::scale(cubeModel, glm::vec3(0.8f, 0.8f, 0.8f));
    updateUniformBuffer(currentFrame, cubeModel, true);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    // Draw second cube
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &tinyBuffer.descriptorSetsCube2[currentFrame], 0, nullptr);
    glm::mat4 cubeModel2 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0, 0));
    cubeModel2 = glm::scale(cubeModel2, glm::vec3(0.8f, 0.8f, 0.8f));
    updateUniformBuffer2(currentFrame, cubeModel2, true);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void TinyEngine::calculateFPS(float deltaTime) {
    frameCount++;
    timeAccumulator += deltaTime;

    
    if (timeAccumulator >= 1.0f) {
        fps = frameCount / timeAccumulator; 
        frameCount = 0; 
        timeAccumulator = 0.0f; 
    }
}

void TinyEngine::lookAround(float deltaTime, float xPos, float yPos) {

    const float constraint = 179.0f;

    camera.yaw += 3 * deltaTime  * xPos;
    camera.pitch += 3 * deltaTime  * yPos; 

    if (camera.yaw > 360) {
        camera.yaw -= 360;
    }

    if (camera.yaw < -360) {
        camera.yaw += 360;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front.y = sin(glm::radians(camera.pitch));
    front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.cameraFront = glm::normalize(front);

}
void TinyEngine::gameUpdate(float deltaTime, TinyWindow& window, TinyInput& input)
{
    const float moveSpeed = 2.8f * deltaTime;

    if (input.keyBoard[TinyButton::A].held) {
        camera.pos.x += moveSpeed;
    }

    if (input.keyBoard[TinyButton::D].held) {
        camera.pos.x -= moveSpeed;
    }

    if (input.keyBoard[TinyButton::W].held) {
        camera.pos.z += moveSpeed;
    }

    if (input.keyBoard[TinyButton::S].held) {
        camera.pos.z -= moveSpeed;
    }

    if (input.keyBoard[TinyButton::Q].held) {
        camera.pos.y += moveSpeed;
    }

    if (input.keyBoard[TinyButton::E].held) {
        camera.pos.y -= moveSpeed;
    }

    if (input.keyBoard[TinyButton::Up].held) {
        glassContainer.z += moveSpeed;
    }

    if (input.keyBoard[TinyButton::Down].held) {
        glassContainer.z -= moveSpeed;
    }

    if (input.keyBoard[TinyButton::Left].held) {
        glassContainer.x += moveSpeed;
    }

    if (input.keyBoard[TinyButton::Right].held) {
        glassContainer.x -= moveSpeed;
    }


    if (input.rightMouse.held) {

        float xPos = input.delta.x;
        float yPos = input.delta.y;

        lookAround(deltaTime, xPos, yPos);
    }


}
void TinyEngine::updateUniformBuffer(uint32_t currentImage, 
    const glm::mat4& modelMatrix, bool useTexture) {

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    TinyBuffer::UniformBufferObject ubo{};
    ubo.model = modelMatrix;

    glm::vec3 pos(0, 1, -2);

    ubo.view = glm::lookAt({ camera.pos },
        glm::vec3(camera.pos) + camera.cameraFront,
        glm::vec3(0.0f, 1.0f, 0.0f));

    ubo.viewPos = glm::vec3(camera.pos);

    ubo.proj = glm::perspective(glm::radians(45.0f), swapChain.getSwapChainExtent().width / (float)swapChain.getSwapChainExtent().height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    memcpy(tinyBuffer.uniformBuffersMappedCube1[currentImage], &ubo, sizeof(ubo));


}

void TinyEngine::updateUniformBuffer2(uint32_t currentImage,
    const glm::mat4& modelMatrix, bool useTexture) {

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    TinyBuffer::UniformBufferObject ubo{};
    ubo.model = modelMatrix;

    glm::vec3 pos(0, 1, -2);

    ubo.view = glm::lookAt({ camera.pos },
        glm::vec3(camera.pos) + camera.cameraFront,
        glm::vec3(0.0f, 1.0f, 0.0f));

    ubo.viewPos = glm::vec3(camera.pos);

    ubo.proj = glm::perspective(glm::radians(45.0f), swapChain.getSwapChainExtent().width / (float)swapChain.getSwapChainExtent().height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    memcpy(tinyBuffer.uniformBuffersMappedCube2[currentImage], &ubo, sizeof(ubo));

}

void TinyEngine::initImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    


    ImGui::StyleColorsDark();

    VkDescriptorPool imguiDescriptorPool;
    VkDescriptorPoolSize poolSizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
    poolInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
    poolInfo.pPoolSizes = poolSizes;
    if (vkCreateDescriptorPool(tinyDevice.getDevice(), &poolInfo, nullptr, &imguiDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create ImGui descriptor pool");
    }


    ImGui_ImplGlfw_InitForVulkan(window.getWindow(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance.getInstance();
    init_info.PhysicalDevice = tinyDevice.physicalDevice;
    init_info.Device = tinyDevice.getDevice();
    init_info.QueueFamily = tinyDevice.findQueueFamilies().graphicsFamily.value();
    init_info.Queue = tinyDevice.getGraphicsQueue();
    init_info.DescriptorPool = imguiDescriptorPool;
    init_info.MinImageCount = 2;
    init_info.ImageCount = swapChain.getSwapChainImages().size();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, swapChain.getRenderPass());

    VkCommandBuffer commandBuffer = command.beginSingleTimeCommands(tinyDevice);
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    command.endSingleTimeCommands(commandBuffer, tinyDevice);
    ImGui_ImplVulkan_DestroyFontUploadObjects();

}

void TinyEngine::drawUI()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("TinyVulcanoEngine");

    ImVec4 pinkColor(1.0f, 0.0f, 1.0f, 1.0f);       
    ImVec4 pinkHoveredColor(1.0f, 0.2f, 1.0f, 1.0f); 

    ImGui::PushStyleColor(ImGuiCol_Button, pinkColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, pinkHoveredColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, pinkColor);

    ImGui::TextColored(pinkColor, "%.2f FPS", fps);
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::Button("Click Me!")) {
        std::cout << "Hey, you just clicked me!" << std::endl;
    }

    ImGui::PopStyleColor(3);
    ImGui::End();

    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

void TinyEngine::loadModel() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    //if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
    //    throw std::runtime_error(warn + err);
    //}

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

            modelIndices.push_back(uniqueVertices[vertex]);
        }
    }
}
