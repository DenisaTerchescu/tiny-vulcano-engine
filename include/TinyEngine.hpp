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

const int MAX_FRAMES_IN_FLIGHT = 2;
const std::string MODEL_PATH = RESOURCES_PATH "models/glass.obj";

//namespace std {
//    template<> struct hash<TinyPipeline::Vertex> {
//        size_t operator()(TinyPipeline::Vertex const& vertex) const {
//            return ((hash<glm::vec3>()(vertex.pos) ^
//                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
//                (hash<glm::vec2>()(vertex.texCoord) << 1);
//        }
//    };
//}

template<> struct hash<TinyPipeline::Vertex> {
    size_t operator()(TinyPipeline::Vertex const& vertex) const {
        return ((hash<glm::vec3>()(vertex.pos) ^
            (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
            (hash<glm::vec2>()(vertex.texCoord) << 1) ^
            (hash<glm::vec3>()(vertex.normal) << 1);  
    }
};

class TinyEngine {
    
    glm::vec3 glassContainer = { 1.25f,0,0 };

    struct Camera {
        float yaw = 90.0f;
        float pitch = 0.0f;
        glm::vec3 pos = { 0, 0.5f, -6 };
        glm::vec3 cameraFront = { 0.0f, 0.0f, 1.0f };
    };


    std::vector<TinyPipeline::Vertex> vertices = {
		{{-1.0f, +1.0f, +1.0f}, // 0
	{0, 1, 0},			 // color
	{0, 0},				 //uv
	{+0.0f, +1.0f, +0.0f}}, // Normal

	{{+1.0f, +1.0f, +1.0f}, // 1
	{0, 1, 0},			 // color
	{1 , 0},				 //uv
	{+0.0f, +1.0f, +0.0f}}, // Normal

	{{+1.0f, +1.0f, -1.0f}, // 2
	{0, 1, 0},			 // color
	{1 , 1 },				 //uv
	{+0.0f, +1.0f, +0.0f}}, // Normal

	{{-1.0f, +1.0f, -1.0f}, // 3
	{0, 1, 0},			 // color
	{0, 1 },				 //uv
	{+0.0f, +1.0f, +0.0f}}, // Normal

	{{-1.0f, +1.0f, -1.0f}, // 4
	{0, 1, 0},			 // color
	{0, 1 },				 //uv
	{0.0f, +0.0f, -1.0f}}, // Normal

	{{+1.0f, +1.0f, -1.0f}, // 5
	{0, 1, 0},			 // color
	 {1 , 1 },				 //uv
		{0.0f, +0.0f, -1.0f}}, // Normal

	{{+1.0f, -1.0f, -1.0f}, // 6
		{0, 1, 0},			 // color
	   {1 , 0},				 //uv
	   {0.0f, +0.0f, -1.0f}}, // Normal

	{{-1.0f, -1.0f, -1.0f}, // 7
		{0, 1, 0},			 // color
	   {0, 0},				 //uv
	  {0.0f, +0.0f, -1.0f}}, // Normal

	{{+1.0f, +1.0f, -1.0f}, // 8
		{0, 1, 0},			 // color
	   {1 , 0},				 //uv
	   {+1.0f, +0.0f, +0.0f}}, // Normal

	{{+1.0f, +1.0f, +1.0f}, // 9
		{0, 1, 0},			 // color
	   {1 , 1 },				 //uv
	   {+1.0f, +0.0f, +0.0f}}, // Normal

	{{+1.0f, -1.0f, +1.0f}, // 10
		{0, 1, 0},			 // color
	   {0, 1 },				 //uv
	   {+1.0f, +0.0f, +0.0f}}, // Normal

	{{+1.0f, -1.0f, -1.0f}, // 11
		{0, 1, 0},			 // color
	   {0, 0},				 //uv
	   {+1.0f, +0.0f, +0.0f}}, // Normal

	{{-1.0f, +1.0f, +1.0f}, // 12
		{0, 1, 0},			 // color
	   {1 , 1 },				 //uv
	   {-1.0f, +0.0f, +0.0f}}, // Normal

	{{-1.0f, +1.0f, -1.0f}, // 13
		{0, 1, 0},			 // color
	   {1 , 0},				 //uv
	   {-1.0f, +0.0f, +0.0f}}, // Normal

	{{-1.0f, -1.0f, -1.0f}, // 14
		{0, 1, 0},			 // color
	   {0, 0},				 //uv
	   {-1.0f, +0.0f, +0.0f}}, // Normal

	{{-1.0f, -1.0f, +1.0f}, // 15
		{0, 1, 0},			 // color
	   {0, 1 },				 //uv
	{-1.0f, +0.0f, +0.0f}}, // Normal

	{{+1.0f, +1.0f, +1.0f}, // 16
		{0, 1, 0},			 // color
	   {1 , 1 },				 //uv
	   {+0.0f, +0.0f, +1.0f}}, // Normal

	{{-1.0f, +1.0f, +1.0f}, // 17
		{0, 1, 0},			 // color
	   {0, 1 },				 //uv
	   {+0.0f, +0.0f, +1.0f}}, // Normal

	{{-1.0f, -1.0f, +1.0f}, // 18
	   {0, 1, 0},			 // color
	   {0, 0},				 //uv
	   {+0.0f, +0.0f, +1.0f}}, // Normal

	{{+1.0f, -1.0f, +1.0f}, // 19
	   {0, 1, 0},			 // color
	   {1 , 0},				 //uv
	   {+0.0f, +0.0f, +1.0f}}, // Normal

		{{+1.0f, -1.0f, -1.0f}, // 20
	   {0, 1, 0},			 // color
	   {1 , 0},				 //uv
	   {+0.0f, -1.0f, +0.0f}}, // Normal

	{{-1.0f, -1.0f, -1.0f}, // 21
   {0, 1, 0},			 // color
   {0, 0},				 //uv
   {+0.0f, -1.0f, +0.0f}}, // Normal

	{{-1.0f, -1.0f, +1.0f}, // 22
   {0, 1, 0},			 // color
   {0, 1 },				 //uv
   {+0.0f, -1.0f, +0.0f}}, // Normal

	{{+1.0f, -1.0f, +1.0f}, // 23
   {0, 1, 0},			 // color
   {1 , 1 },				 //uv
{+0.0f, -1.0f, +0.0f}}, // Normal
    };



	std::vector<uint32_t> indices = {
		16, 17, 18, 16, 18, 19, // Front
4,   5,  6,  4,  6,  7, // Back
0,   1,  2,  0,  2,  3, // Top
20, 22, 21, 20, 23, 22, // Bottom
12, 13, 14, 12, 14, 15, // Left
8,   9, 10,  8, 10, 11, // Right
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

	//std::vector<TinyPipeline::Vertex> vertices;
	//std::vector<uint32_t> indices;
	//VkBuffer modelVertexBuffer;
	//VkDeviceMemory modelVertexBufferMemory;

    TinySync tinySync;

    TinyBuffer tinyBuffer;

    TinyDepth depth;

    TinyTexture texture;
    Camera camera;
   
    uint32_t currentFrame = 0;

    void initVulkan();

    void mainLoop();

    void drawFrame();

    void cleanup();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void gameUpdate(float deltaTime, TinyWindow& window, TinyInput& input);

    void updateUniformBuffer(uint32_t currentImage, const glm::mat4& modelMatrix, bool useTexture = false);
    void updateUniformBuffer2(uint32_t currentImage, const glm::mat4& modelMatrix, bool useTexture = false);

    void lookAround(float deltaTime, float xPos, float yPos);

    void calculateFPS(float deltaTime);

    void initImgui();

    void SetupImGuiStyle();
    
    void drawUI();

	void loadModel();

};
