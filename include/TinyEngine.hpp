#pragma once
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

//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>

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

class TinyEngine {
    glm::vec3 glassContainerPos = { 0,0,0 };

    struct Camera {
        float yaw = 90.0f;
        float pitch = 0.0f;
        glm::vec3 pos = { 0, 0.5f, -6 };
        glm::vec3 cameraFront = { 0.0f, 0.0f, 1.0f };
    };

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

    int frameCount = 0;
    float fps = 0.0f;
    float fpsUpdateInterval = 1.0f; 
    float timeAccumulator = 0.0f;
public:
    void run();

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
    Camera camera;

    //std::vector<Vertex> vertices;
    //std::vector<uint32_t> indices;
   
    uint32_t currentFrame = 0;

    void initVulkan();

    void mainLoop();

    void drawFrame();

    void cleanup();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    //void loadModel();

    void gameUpdate(float deltaTime, TinyWindow& window, TinyInput& input);

    void updateUniformBuffer(uint32_t currentImage, const glm::mat4& modelMatrix, bool useTexture = false);

    void lookAround(float deltaTime, float xPos, float yPos);

    void calculateFPS(float deltaTime);

};
