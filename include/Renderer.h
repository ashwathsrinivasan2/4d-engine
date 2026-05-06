#ifndef RENDERER_H
#define RENDERER_H

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan_macos.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>
#include <algorithm>
#include <fstream>
#include <array>
#include <chrono>
            
#include "Scene.h"

class Renderer{

    const int MAX_FRAMES_IN_FLIGHT = 2; //how many frames GPU should process at a time

    const std::vector<const char *> validationLayers = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char *> deviceExtensions = 
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        "VK_EXT_mesh_shader"
    };

    #ifdef NDEBUG
    const bool enableValidationLayers = false;
    #else
    const bool enableValidationLayers = true;
    #endif

    struct QueueFamilyIndices
    {
        //may or may not have a value
        std::optional<unsigned int> graphicsFamily;
        std::optional<unsigned int> presentFamily;
        std::optional<unsigned int> transferFamily;

        //true if values are set
        bool isCompleted()
        {
            return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
        }
    };

    struct SwapchainSupportDetails{
        VkSurfaceCapabilitiesKHR capabilities; //device/surface extent size
        std::vector<VkSurfaceFormatKHR> formats; //pixel format, color space
        std::vector<VkPresentModeKHR> presentModes; //how are frames displayed (FIFO, triple buffering, etc.)
    };

    struct UniformBufferObject{
        glm::mat4 view;
        glm::vec4 viewTranslate;
        glm::mat4 proj;
        unsigned numTetrahedrons;
    };

    struct InstanceData {
        glm::mat4 model;
        glm::vec4 modelTranslate;
    };

    bool                            framebufferResized          = false;

    GLFWwindow*                     window;
    VkInstance                      instance;

    VkDebugUtilsMessengerEXT        debugMessenger;
    VkSurfaceKHR                    surface;
    VkPhysicalDevice                physicalDevice;
    VkDevice                        logicalDevice;

    VkQueue                         graphicsQueue;
    VkQueue                         presentQueue;
    VkQueue                         transferQueue;

    VkSwapchainKHR                  swapchain;
    std::vector<VkImage>            swapchainImages;
    std::vector<VkImageView>        swapchainImageViews;
    VkExtent2D                      swapchainExtent;
    VkFormat                        swapchainImageFormat;

    std::vector<VkImage>            depthImages;
    VkFormat                        depthFormat;
    std::vector<VkDeviceMemory>     depthImagesMemory;
    std::vector<VkImageView>        depthImageViews;

    std::string                     taskShaderFilename          = "shaders/task.spv";
    VkShaderModule                  taskShaderModule;
    std::string                     meshShaderFilename          = "shaders/mesh.spv";
    VkShaderModule                  meshShaderModule;
    std::string                     fragShaderFilename          = "shaders/frag.spv";
    VkShaderModule                  fragShaderModule;


    VkCommandPool                   graphicsCommandPool;
    VkCommandPool                   transferCommandPool;

    std::vector<VkBuffer>           uniformBuffers;
    std::vector<VkDeviceMemory>     uniformBuffersMemory;
    std::vector<void*>              uniformBuffersMapped;

    VkBuffer                        vertexSSBO;
    VkDeviceMemory                  vertexSSBOMemory;
    VkBuffer                        instanceSSBO;
    VkDeviceMemory                  instanceSSBOMemory;

    VkDescriptorSetLayout           descriptorSetLayout;
    VkDescriptorPool                descriptorPool;
    std::vector<VkDescriptorSet>    descriptorSets;

    VkPipelineLayout                pipelineLayout;
    VkPipeline                      pipeline;

    std::vector<VkCommandBuffer>    commandBuffers;

    std::vector<VkSemaphore>        imageAvailableSemaphores;
    std::vector<VkSemaphore>        renderFinishedSemaphores;
    std::vector<VkFence>            inFlightFences;
    unsigned long long              currentFrame                = 0;

    Scene* scene;

    VkResult createDebugUtilMessengerEXT(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
    void destroyDebugUtilMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);

    void vkCmdDrawMeshTasksEXT(VkCommandBuffer, int, int, int);

    //instance helpers
    bool checkValidationLayerSupport();
    const std::vector<const char*> getRequiredExtensions();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);

    //physical device helpers
    bool isDeviceSuitable(VkPhysicalDevice);
    bool checkDeviceExtensionSupport(VkPhysicalDevice);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice);
    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();

    //swapchain helpers
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&);

    //shader helpers
    static std::vector<char> readFile(const std::string&);
    VkShaderModule createShaderModule(const std::vector<char>&);

    //buffer helpers
    void createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
    void copyBuffer(VkBuffer, VkBuffer, VkDeviceSize);
    unsigned int findMemoryType(unsigned int, VkMemoryPropertyFlags);

    VkImageView createImageView(VkImage, VkFormat, VkImageAspectFlagBits);
    void transitionImageLayout(VkCommandBuffer, VkImage, VkFormat, VkImageLayout, VkImageLayout);
    void createImage(uint32_t, uint32_t, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, VkImage&, VkDeviceMemory&);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer);

    template<typename T>
    void createSSBO(size_t size, std::vector<T> sentData, VkBuffer buffer, VkDeviceMemory bufferMemory) {
        VkDeviceSize bufferSize = size;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, sentData.data(), (size_t)bufferSize);
        vkUnmapMemory(logicalDevice, stagingBufferMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);
    }

    //clean up
    void cleanUpSwapchain();
    void cleanUp(Scene&);
    void recreateSwapchain();

    //initialization helpers
    void createInstance();
    void createDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapchain();
    void createImageViews();
    void createShaderModules();
    void createCommandPools();
    void createShaderStorageBufferObjects();
    void createUniformBuffers();
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();
    void createPipeline();
    void createCommandBuffers();
    void createSyncObjects();
    void createDepthResources();

    void updateUniformBuffer(uint32_t);



    public:
    Renderer();
    ~Renderer();
    void initialize(GLFWwindow*, Scene*);

    void drawFrame();
    void recordCommandBuffers(int i);

    VkCommandPool& getCommandPool(){return transferCommandPool;}
    VkQueue& getTransferQueue(){return transferQueue;}
    VkDevice& getLogicalDevice(){return logicalDevice;}
    VkPhysicalDevice& getPhysicalDevice(){return physicalDevice;}
};

#endif