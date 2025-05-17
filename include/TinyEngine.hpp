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
#include <string>

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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



const int MAX_FRAMES_IN_FLIGHT = 2;
const std::string BALL_MODEL_PATH = RESOURCES_PATH "models/sphere.glb";
const std::string PINGUIN_MODEL_PATH = RESOURCES_PATH "models/pinguin.glb";

//const std::string GLASS_TEXTURE_PATH = RESOURCES_PATH "textures/glass.png";
//const std::string PINK_TEXTURE_PATH = RESOURCES_PATH "textures/pink.jpg";
//const std::string CUTE_PINK_TEXTURE_PATH = RESOURCES_PATH "textures/cute_pink.jpg";
//const std::string FLOOR_TEXTURE_PATH = RESOURCES_PATH "textures/pink_stone.png";
//const std::string PURPLE_TEXTURE_PATH = RESOURCES_PATH "textures/green_marble.jpg";
const std::string PBR_TEXTURE_PATH = RESOURCES_PATH "textures/textureMap2.jpg";
const std::string PBR_DIFFUSE_TEXTURE_PATH = RESOURCES_PATH "textures/textMapDiffuse.jpg";
const std::string NORMAL_TEXTURE_PATH = RESOURCES_PATH "textures/textMapDiffuse.jpg";

template<> struct hash<TinyPipeline::Vertex> {
    size_t operator()(TinyPipeline::Vertex const& vertex) const {
        return ((hash<glm::vec3>()(vertex.pos) ^
            (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
            (hash<glm::vec2>()(vertex.texCoord) << 1) ^
            (hash<glm::vec3>()(vertex.normal) << 1);  
    }
};

class TinyEngine {

	glm::vec3 glassContainer = { 2.5f,0,0 };
	glm::vec3 spherePosition = glm::vec3(-1.5f, 0, 0);
	glm::vec3 spherePosition2 = glm::vec3(1.25f, 0, 0);
	glm::vec3 secondCube = glm::vec3(3.0f, 0, 0);
	bool secondCubeShown = false;
	bool secondSphereShown = false;

	struct Camera {
		float yaw = 90.0f;
		float pitch = 0.0f;
		glm::vec3 pos = { 0, 0.5f, -6 };
		glm::vec3 cameraFront = { 0.0f, 0.0f, 1.0f };
	};

	struct Cube {
		glm::vec3 min;
		glm::vec3 max;
	};

	struct Sphere {
		glm::vec3 center;
		float radius;
	};

	struct TinyModel {
		std::vector<TinyPipeline::Vertex> vertices;
		std::vector<uint32_t> indices;

		VkBuffer modelVertexBuffer;
		VkDeviceMemory modelVertexBufferMemory;
		VkBuffer modelIndexBuffer;
		VkDeviceMemory modelIndexBufferMemory;
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


	std::vector<TinyPipeline::Vertex> planeVertices = {
	{{-0.5f, 0.0f, -0.5f}, {0, 1, 0}, {0.0f, 0.0f}},
	{{ 0.5f, 0.0f, -0.5f}, {0, 1, 0}, {1.0f, 0.0f}},
	{{ 0.5f, 0.0f,  0.5f}, {0, 1, 0}, {1.0f, 1.0f}},
	{{-0.5f, 0.0f,  0.5f}, {0, 1, 0}, {0.0f, 1.0f}},
	};

	std::vector<uint32_t> planeIndices = {
		0, 3, 2, 2, 1, 0
	};


	int frameCount = 0;
	float fps = 0.0f;
	float fpsUpdateInterval = 1.0f;
	float timeAccumulator = 0.0f;
	string collisionDetectedText = "";

public:
    void run();

    TinyWindow window;
    TinyInstance instance;
    TinyDevice tinyDevice;
    TinySwapChain swapChain;
    TinyPipeline pipeline;
    TinyCommand command;

	std::vector<TinyModel> models;

    TinySync tinySync;

    TinyBuffer tinyBuffer;

    TinyDepth depth;

    TinyTexture pbrDiffuseTexture;
    TinyTexture roughnessTexture;
    TinyTexture normalTexture;

    Camera camera;
   
    uint32_t currentFrame = 0;

	float loadingTime = 0.0f;


    void initVulkan();

    void mainLoop();

    void drawFrame();

	void loadModelAssimp(const std::string modelPath);

	void processNode(aiNode* node, const aiScene* scene,
		std::unordered_map<TinyPipeline::Vertex, uint32_t>& uniqueVertices,
		std::vector<TinyPipeline::Vertex>& vertices,
		std::vector<uint32_t>& indices);

	void processMesh(aiMesh* mesh, const aiScene* scene,
		std::unordered_map<TinyPipeline::Vertex, uint32_t>& uniqueVertices,
		std::vector<TinyPipeline::Vertex>& vertices,
		std::vector<uint32_t>& indices);

    void cleanup();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void gameUpdate(float deltaTime, TinyWindow& window, TinyInput& input);

	TinyBuffer::UniformBufferObject  updateUniformBuffer();

    void lookAround(float deltaTime, float xPos, float yPos);

    void calculateFPS(float deltaTime);

    void initImgui();

    void SetupImGuiStyle();
    
    void drawUI();


	bool CheckCollisionSphere(const Sphere& sphere1, const Sphere& sphere2);

	bool CheckCollisionAABB(const Cube& cube1, const Cube& cube2);

	bool CheckCollisionAABBSphere(const Cube& cube, const Sphere& spherePosition);



};
