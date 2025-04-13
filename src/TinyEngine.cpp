#define GLM_ENABLE_EXPERIMENTAL
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include "TinyEngine.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool TinyEngine::CheckCollisionSphere(const Sphere& sphere1, const Sphere& sphere2) {
    float dist = glm::distance(sphere1.center, sphere2.center);
    float radiusSum = sphere1.radius + sphere2.radius;
    return dist < radiusSum;
}

bool TinyEngine::CheckCollisionAABB(const Cube& cube1, const Cube& cube2) {
    if (cube1.max.x < cube2.min.x || cube1.min.x > cube2.max.x) return false;
    if (cube1.max.y < cube2.min.y || cube1.min.y > cube2.max.y) return false;
    if (cube1.max.z < cube2.min.z || cube1.min.z > cube2.max.z) return false;
    return true;
}

bool TinyEngine::CheckCollisionAABBSphere(const Cube& cube, const Sphere& spherePosition) {
    glm::vec3 closestPoint = glm::clamp(spherePosition.center, cube.min, cube.max);
    float distance = glm::distance(spherePosition.center, closestPoint);
    return distance < spherePosition.radius;
}


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
    swapChain.init(tinyDevice, window.getWindow(), depth);
    pipeline.init(tinyDevice, swapChain);
    command.createCommandPool(tinyDevice);
    depth.createDepthResources(tinyDevice, command, swapChain, texture);
    swapChain.createFramebuffers(tinyDevice, depth);
    texture.init(tinyDevice, command, tinyBuffer);
    loadModelAssimp();
    tinyBuffer.createVertexBuffer(tinyDevice, command, vertices, tinyBuffer.vertexBuffer, tinyBuffer.vertexBufferMemory);
    tinyBuffer.createIndexBuffer(tinyDevice, command, indices,tinyBuffer.indexBuffer, tinyBuffer.indexBufferMemory);
    tinyBuffer.createVertexBuffer(tinyDevice, command, modelVertices, tinyBuffer.modelVertexBuffer, tinyBuffer.modelVertexBufferMemory);
    tinyBuffer.createIndexBuffer(tinyDevice, command, modelIndices, tinyBuffer.modelIndexBuffer, tinyBuffer.modelIndexBufferMemory);
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

        Cube cube1 = { glm::vec3(-0.8f, -0.8f, -0.8f) + glassContainer, glm::vec3(0.8f, 0.8f, 0.8f) + glassContainer};
        Cube cube2 = { glm::vec3(-0.8f, -0.8f, -0.8f) + secondCube, glm::vec3(0.8f, 0.8f, 0.8f) + secondCube};
        Sphere sphere1 = { spherePosition, 0.5f};
        Sphere sphere2 = { spherePosition2, 0.5f};

        if (secondCubeShown) {
            if (CheckCollisionAABB(cube1, cube2)) {
                collisionDetectedText = "Collision detected!";
            }
            else {
                collisionDetectedText = "No collision!!";
            }
        }
        else if (secondSphereShown) {
            if (CheckCollisionSphere(sphere1, sphere2)) {
                collisionDetectedText = "Collision detected!";
            }
            else {
                collisionDetectedText = "No collision!!";
            }
        }
        else {
            if (CheckCollisionAABBSphere(cube1, sphere1)) {
                collisionDetectedText = "Collision detected!";
            }
            else {
                collisionDetectedText = "No collision!!";
            }
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

void TinyEngine::loadModel() {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        std::cout << "Loading model didn't work!";
        return;
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

            if (!attrib.normals.empty()) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            if (!attrib.texcoords.empty())
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(modelVertices.size());
                modelVertices.push_back(vertex);
            }

            modelIndices.push_back(uniqueVertices[vertex]);
        }
    }
}

void TinyEngine::loadModelAssimp() {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        MODEL_PATH,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::unordered_map<TinyPipeline::Vertex, uint32_t> uniqueVertices{};
    processNode(scene->mRootNode, scene, uniqueVertices);
}

void TinyEngine::processNode(aiNode* node, const aiScene* scene,
    std::unordered_map<TinyPipeline::Vertex, uint32_t>& uniqueVertices) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, uniqueVertices);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, uniqueVertices);
    }
}

void TinyEngine::processMesh(aiMesh* mesh, const aiScene* scene,
    std::unordered_map<TinyPipeline::Vertex, uint32_t>& uniqueVertices) {
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        TinyPipeline::Vertex vertex{};

        // Position
        vertex.pos = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        // Normals
        if (mesh->HasNormals()) {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        // TexCoords (only the first set)
        if (mesh->mTextureCoords[0]) {
            vertex.texCoord = {
                mesh->mTextureCoords[0][i].x,
                1.0f - mesh->mTextureCoords[0][i].y // Flip V
            };
        }

        vertex.color = { 1.0f, 1.0f, 1.0f }; // default color

        if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32_t>(modelVertices.size());
            modelVertices.push_back(vertex);
        }
    }

    // Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            modelIndices.push_back(uniqueVertices[modelVertices[face.mIndices[j]]]);
        }
    }
}


void TinyEngine::drawFrame() {
    vkWaitForFences(tinyDevice.getDevice(), 1, &tinySync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(tinyDevice.getDevice(), swapChain.getSwapChain(), UINT64_MAX, tinySync.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.framebufferResized) {
        window.framebufferResized = false;
        swapChain.recreateSwapChain(tinyDevice, window, depth);
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

   /* VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;*/

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

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

    //VkBuffer vertexBuffers[] = { tinyBuffer.vertexBuffer };
    VkDeviceSize offsets[] = { 0, 0 };
    //vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    //vkCmdBindIndexBuffer(commandBuffer, tinyBuffer.indexBuffer, 0, VK_INDEX_TYPE_UINT32);



    // Drawing the cube
     vkCmdBindVertexBuffers(commandBuffer, 0, 1, &tinyBuffer.vertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, tinyBuffer.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, 
        &tinyBuffer.descriptorSets[0][currentFrame], 0, nullptr);
    glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), glm::vec3(glassContainer.x, glassContainer.y, glassContainer.z));
    cubeModel = glm::scale(cubeModel, glm::vec3(0.8f, 0.8f, 0.8f));
    updateUniformBuffer(0, currentFrame, cubeModel, true);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);


    // Drawing the second cube
    //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &tinyBuffer.descriptorSetsCube2[currentFrame], 0, nullptr);
    //glm::mat4 cubeModel2 = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0, 0));
    //cubeModel2 = glm::scale(cubeModel2, glm::vec3(0.8f, 0.8f, 0.8f));
    //updateUniformBuffer2(currentFrame, cubeModel2, true);
    //vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    //secondCubeShown = true;


        // Drawing the second sphere model
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &tinyBuffer.modelVertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, tinyBuffer.modelIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

    //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &tinyBuffer.descriptorSetsCube1[currentFrame], 0, nullptr);
    //vkCmdBindIndexBuffer(commandBuffer, tinyBuffer.modelIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &tinyBuffer.descriptorSetsCube1[currentFrame], 0, nullptr);
    //glm::mat4 sphereModel2 = glm::translate(glm::mat4(1.0f), spherePosition2);
    //sphereModel2 = glm::scale(sphereModel2, glm::vec3(1.2f, 1.2f, 1.2f));
    //updateUniformBuffer(currentFrame, sphereModel2, true);
    //vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(modelIndices.size()), 1, 0, 0, 0);
    //secondSphereShown = true;

    // Drawing the sphere model
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &tinyBuffer.modelVertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, tinyBuffer.modelIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout,
        0, 1, &tinyBuffer.descriptorSets[1][currentFrame], 0, nullptr);
    glm::mat4 sphereModel = glm::translate(glm::mat4(1.0f), spherePosition);
    sphereModel = glm::scale(sphereModel, glm::vec3(1.2f, 1.2f, 1.2f));
    updateUniformBuffer(1,currentFrame, sphereModel, true);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(modelIndices.size()), 1, 0, 0, 0);

    // Drawing the third object
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &tinyBuffer.modelVertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, tinyBuffer.modelIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout,
        0, 1, &tinyBuffer.descriptorSets[2][currentFrame], 0, nullptr);
    glm::mat4 sphereModel2 = glm::translate(glm::mat4(1.0f), spherePosition + glm::vec3(1,0,0));
    sphereModel = glm::scale(sphereModel2, glm::vec3(0.5f, 0.5f, 0.5f));
    updateUniformBuffer(2, currentFrame, sphereModel2, true);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(modelIndices.size()), 1, 0, 0, 0);


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
void TinyEngine::updateUniformBuffer(uint32_t objectIndex, uint32_t currentImage,
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

    memcpy(tinyBuffer.uniformBuffersMapped[objectIndex][currentImage], &ubo, sizeof(ubo));


}


void TinyEngine::initImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    


    SetupImGuiStyle();

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

    ImGui::Text("%.2f FPS", fps);
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), collisionDetectedText.c_str());
    ImGui::Spacing();

    ImGui::DragFloat3("Sphere position", &spherePosition[0], 0.1f);
    ImGui::DragFloat3("Cube position", &glassContainer[0], 0.1f);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Text("WASDEQ - Move camera");
    ImGui::Text("Right click - Look around");
    ImGui::Text("Arrows - Move the cube");
    ImGui::End();

    ImGui::Render();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}
void TinyEngine::SetupImGuiStyle()
{
    // Soft Cherry style by Patitotective from ImThemes
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.4000000059604645f;
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.WindowRounding = 4.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowMinSize = ImVec2(50.0f, 50.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(5.0f, 3.0f);
    style.FrameRounding = 3.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(6.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(3.0f, 2.0f);
    style.CellPadding = ImVec2(3.0f, 3.0f);
    style.IndentSpacing = 6.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 13.0f;
    style.ScrollbarRounding = 16.0f;
    style.GrabMinSize = 20.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.TabBorderSize = 1.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5215686559677124f, 0.5490196347236633f, 0.5333333611488342f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1294117718935013f, 0.1372549086809158f, 0.168627455830574f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1490196138620377f, 0.1568627506494522f, 0.1882352977991104f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.1372549086809158f, 0.1137254908680916f, 0.1333333402872086f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.168627455830574f, 0.1843137294054031f, 0.2313725501298904f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2313725501298904f, 0.2000000029802322f, 0.2705882489681244f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.501960813999176f, 0.07450980693101883f, 0.2549019753932953f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.239215686917305f, 0.239215686917305f, 0.2196078449487686f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3725490272045135f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.6941176652908325f, 0.6941176652908325f, 0.686274528503418f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6941176652908325f, 0.6941176652908325f, 0.686274528503418f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.658823549747467f, 0.1372549086809158f, 0.1764705926179886f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.6509804129600525f, 0.1490196138620377f, 0.3450980484485626f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.7098039388656616f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.6509804129600525f, 0.1490196138620377f, 0.3450980484485626f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.6509804129600525f, 0.1490196138620377f, 0.3450980484485626f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.501960813999176f, 0.07450980693101883f, 0.2549019753932953f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.6509804129600525f, 0.1490196138620377f, 0.3450980484485626f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.3098039329051971f, 0.7764706015586853f, 0.196078434586525f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.3843137323856354f, 0.6274510025978088f, 0.9176470637321472f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 1.0f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.300000011920929f);
}

