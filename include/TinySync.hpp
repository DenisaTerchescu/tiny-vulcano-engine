#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include "TinyDevice.hpp"
#include "TinySwapchain.hpp"
#include "TinyBuffer.hpp"
#include "TinyCommand.hpp"


class TinySync {
public:
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    VkFence inFlightFence;

    const int MAX_FRAMES_IN_FLIGHT = 2;

    void createSyncObjects(TinyDevice& device);
    void cleanup(TinyDevice& device);
};


