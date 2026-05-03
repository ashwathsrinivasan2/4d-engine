#include "Renderer.h"
#include "Scene.h"

#include <cstdlib> 
#include <ctime>

Renderer::Renderer(){}

Renderer::~Renderer(){
    int swapchainSize = swapchainImages.size();
    cleanUpSwapchain();

    for(int i = 0; i < swapchainSize; i++){
        vkDestroyImageView(logicalDevice, depthImageViews[i], nullptr);
        vkDestroyImage(logicalDevice, depthImages[i], nullptr);
        vkFreeMemory(logicalDevice, depthImagesMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkDestroyBuffer(logicalDevice, uniformBuffers[i], nullptr);
        vkFreeMemory(logicalDevice, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyBuffer(logicalDevice, vertexSSBO, nullptr);
    vkFreeMemory(logicalDevice, vertexSSBOMemory, nullptr);
    vkDestroyBuffer(logicalDevice, instanceSSBO, nullptr);
    vkFreeMemory(logicalDevice, instanceSSBOMemory, nullptr);


    for(unsigned long long i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
    }

    for (unsigned long long i = 0; i < swapchainSize; i++) {
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
    }



    vkDestroyCommandPool(logicalDevice, transferCommandPool, nullptr);
    vkDestroyCommandPool(logicalDevice, graphicsCommandPool, nullptr);
    
    vkDestroyDevice(logicalDevice, nullptr);

    if(enableValidationLayers){
        destroyDebugUtilMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

//create a debug messenger object from a debug create info
VkResult Renderer::createDebugUtilMessengerEXT(VkInstance instance, 
                                    const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, 
                                    const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger){

    //getInstanceProc gets a function from one of the extensions
    //PFN... casts it to a c++ style function pointer specifically built for this function (argument count and types)
    //func(...) then calls that function if it is found in the extensions
    //this allows you to call the function even though it is not included in Vulkan's core api
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr){
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


void Renderer::vkCmdDrawMeshTasksEXT(VkCommandBuffer cmdBuffer, int x, int y, int z) {

    //getInstanceProc gets a function from one of the extensions
    //PFN... casts it to a c++ style function pointer specifically built for this function (argument count and types)
    //func(...) then calls that function if it is found in the extensions
    //this allows you to call the function even though it is not included in Vulkan's core api
    auto func = (PFN_vkCmdDrawMeshTasksEXT)vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMeshTasksEXT");
    if (func != nullptr) {
        func(cmdBuffer, x, y, z);
    }
    else {
        throw std::runtime_error("Failed to load vkCmdDrawMeshTasksEXT");
    }
}
//deallocates a debug messenger object
void Renderer::destroyDebugUtilMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr){
        func(instance, debugMessenger, pAllocator);
    }
}

//instance helpers
bool Renderer::checkValidationLayerSupport(){
    unsigned int layerCount;

    //count number of available layers provided by system's Vulkan implementation
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);

    //same thing, but store layer structs in availableLayers
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    //checks that every layerName in validationLayers is available on the system
    for(const char *layerName : validationLayers){
        bool layerFound = false;

        for(const auto &layerProperties: availableLayers){
            if(strcmp(layerName, layerProperties.layerName) == 0){
                layerFound = true;
                break;
            }
        }
        if(!layerFound)return false;
    }

    return true;
}

const std::vector<const char*> Renderer::getRequiredExtensions(){
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); //all extensions needed for window interactions with glfw

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME); //more detailed physical device info

    if(enableValidationLayers){
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); //allows debugging
    }

    return extensions;
}

void Renderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo){
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    //categories of severity to include in debug log  (high detail messages, info messages, warnings, errors)
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    
    //categories of types of messages to include in debug log (general messages, api misuse, performance issues)
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    
    //function ptr defining what to do when bug is found (log, quit, etc.)
    createInfo.pfnUserCallback = debugCallback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData){
    std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl << std::endl;

    return VK_FALSE; //continues normally, VK_TRUE would abort the call
}

//physical device helpers
bool Renderer::isDeviceSuitable(VkPhysicalDevice physicalDevice){
    //check if required queue families are available
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    //check if device extensions are available
    bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

    //check if physical device/surface has at least one available format and presentMode
    bool swapchainAdequate = false;
    if (extensionsSupported)
    {
        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice);
        swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
    }

    return indices.isCompleted() && extensionsSupported && swapchainAdequate;
}

bool Renderer::checkDeviceExtensionSupport( VkPhysicalDevice physicalDevice )
{
    //fill vector with all of device's available extensions
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, nullptr );

    std::vector<VkExtensionProperties> availableExtensions( extensionCount );
    vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, availableExtensions.data( ) );

    //copy unique elements of deviceExtensions to requiredExtensions
    std::set<std::string> requiredExtensions( deviceExtensions.begin( ), deviceExtensions.end( ) );

    //ensures every element from reqExtensions is an available extension
    for ( const auto &extension : availableExtensions )
    {
        requiredExtensions.erase( extension.extensionName );
    }

    return requiredExtensions.empty( );
}

Renderer::QueueFamilyIndices Renderer::findQueueFamilies(VkPhysicalDevice physicalDevice){
    QueueFamilyIndices indices;
    unsigned int queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

    //collect all queue family properties in a vector
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    //check each queue family in queueFamilies that can perform rendering
    //if 
    for(const auto &queueFamily: queueFamilies){

        //find queueFamily that can render (graphics bit)
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
            indices.transferFamily = i;
        } 

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if(presentSupport){
            indices.presentFamily = i;
        }

        //separate isCompleted check to allow for more queueFamilies to be added to QueueFamilyIndices in the future
        if(indices.isCompleted()){
            break;
        }
        i++;
    }

    return indices;
}

Renderer::SwapchainSupportDetails Renderer::querySwapchainSupport( VkPhysicalDevice device ){
    SwapchainSupportDetails details;

    //get GPU and surface requirements for swapchain image size
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, surface, &details.capabilities );


    //fill details.formats with all formats supported by the physical device and surface
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, nullptr );

    if ( formatCount != 0 )
    {
        details.formats.resize( formatCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( device, surface, &formatCount, details.formats.data( ) );
    }

    //fill details.presentModes with all presentModes supported by the physical device and surface
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, nullptr );

    if ( presentModeCount != 0 )
    {
        details.presentModes.resize( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( device, surface, &presentModeCount, details.presentModes.data( ) );
    }

    return details;
}

//depth resources helpers
VkFormat Renderer::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features){
    for(VkFormat format : candidates){
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
            return format;
        } else {
            throw std::runtime_error("Failed to find supported format");
        }
    }
}

VkFormat Renderer::findDepthFormat(){
    return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, 
                                VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

//swapchain helpers
VkSurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for ( const auto &availableFormat : availableFormats )
    {
        if ( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
        {
            return availableFormat;
        }
    }

    //return the first available format if a desired one is not found (one is guaranteed)
    return availableFormats[0];
}

VkPresentModeKHR Renderer::chooseSwapPresentMode( const std::vector<VkPresentModeKHR> &availablePresentModes )
{
    for ( const auto &availablePresentMode : availablePresentModes )
    {
        //search for mailbox present mode (triple buffering, low latency)
        if ( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
        {
            return availablePresentMode;
        }
    }

    //if mailbox is not found, return fifo (guaranteed)
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::chooseSwapExtent( const VkSurfaceCapabilitiesKHR &capabilities )
{
    if ( capabilities.currentExtent.width != UINT32_MAX )
    {
        //return extent size if one is given in capabilities.
        return capabilities.currentExtent;
    }
    else
    {
        //set extent size to glfw window size
        int width, height;
        glfwGetFramebufferSize( window, &width, &height );

        VkExtent2D actualExtent =
        {
            static_cast< uint32_t >( width ),
            static_cast< uint32_t >( height )
        };

        //clamp extend size to devices min/max size
        actualExtent.width = std::clamp( actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
        actualExtent.height = std::clamp( actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );

        return actualExtent;
    }
}

//shader helpers
std::vector<char> Renderer::readFile(const std::string &filename){
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if(!file.is_open()){
        throw std::runtime_error("Failed to open file");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

VkShaderModule Renderer::createShaderModule(const std::vector<char> &code){
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();

    //reinterpret_cast used because dealing with raw byte data (1 byte char -> 32 bit unsigned int)
    createInfo.pCode = reinterpret_cast<const unsigned int*>(code.data());

    VkShaderModule shaderModule;   
    if(vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS){
        throw std::runtime_error("Failed to create shader module.");
    }

    return shaderModule;
}

//buffer helpers
void Renderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory){
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage; //this affects buffer offset/alignment/where data begins in buffer
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS){
        throw std::runtime_error("Failed to create buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;   //may have additional memory needs to store buffer in device memory (padding, extra info, etc.)
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties); //memoryTypeBits is a bit string representing which memory type indices are suitable for the buffer

    if(vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate vertex buffer memory");
    }
    vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

void Renderer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size){ 
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    
    endSingleTimeCommands(commandBuffer);
}

unsigned int Renderer::findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties){
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);                    //query physical device for memory types it can allocate from

    for(unsigned int i = 0; i < memProperties.memoryTypeCount; i++){                        //iterates through all memory types
        if((typeFilter & (1 << i))                                                          //checks if type filter says memory type is valid
            && (memProperties.memoryTypes[i].propertyFlags & properties) == properties){    //checks if memory type supports all properties
            return i;                                                                       //returns first memory type supporting device, buffer, and given properties
        }
    }

    throw std::runtime_error("Failed to find suitable memory type");
}

//image helpers
void Renderer::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(logicalDevice, image, imageMemory, 0);
}

void Renderer::transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {


    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL){
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR){
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;

        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

}

VkImageView Renderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspectFlags) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    return imageView;
}

//command buffer helpers
VkCommandBuffer Renderer::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = transferCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void Renderer::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(logicalDevice, transferCommandPool, 1, &commandBuffer);
}

//clean up
void Renderer::cleanUpSwapchain(){

    vkFreeCommandBuffers(logicalDevice, graphicsCommandPool, static_cast<unsigned int>(commandBuffers.size()), commandBuffers.data());

    vkDestroyPipeline(logicalDevice, pipeline, nullptr);
    vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
    for(auto imageView : swapchainImageViews){
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);
}

void Renderer::recreateSwapchain(){
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    while(width == 0 || height == 0){
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(logicalDevice);

    cleanUpSwapchain();
    createSwapchain();
    createImageViews();
    createPipeline();
    createCommandBuffers();
}

//initialization helpers
void Renderer::createInstance(){
    if(enableValidationLayers && !checkValidationLayerSupport()){
        throw std::runtime_error("Validation layers requested, but not available.");
    }

    //appInfo - details about app, engine, version 
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

    //createInfo - details about extensions, flags, validation layers (includes appInfo)
    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    //fill a vector of extension names and then get its size and data to put in createInfo
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<unsigned int> (extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkValidationFeaturesEXT validationFeatures{};
    validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;

    VkValidationFeatureEnableEXT enables[] = {
        VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT
    };
    validationFeatures.enabledValidationFeatureCount = 1;
    validationFeatures.pEnabledValidationFeatures = enables;

    // chain into your instance create info
    createInfo.pNext = &validationFeatures;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if(enableValidationLayers){ //meant for specific debug logs
        createInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        validationFeatures.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        validationFeatures.pNext = nullptr;
    }


    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        throw std::runtime_error("Failed to create Vulkan Instance");
    }
}

void Renderer::createDebugMessenger(){
    if(!enableValidationLayers) return;

    //creates and fills a debug createInfo struct
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    //creates debug object (debugMessenger) from createInfo struct
    if(createDebugUtilMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS){
        throw std::runtime_error("Failed to set up debug messenger.");
    }
}

void Renderer::createSurface(){
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
        throw std::runtime_error("Failed to create a window surface");
    }
}

void Renderer::pickPhysicalDevice(){
    unsigned int deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr); //counts all GPUs accessible by the system
    if(deviceCount == 0){
        throw std::runtime_error("Failed to find any GPUs with Vulkan");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()); //store all accessible GPUs in devices
    for(const auto &device : devices){  //find and use first GPU that works 
        if(isDeviceSuitable(device)){
            physicalDevice = device;
            break;
        }
    }

    if(physicalDevice == VK_NULL_HANDLE){
        throw std::runtime_error("Failed to find a suitable GPU");
    }
}

void Renderer::createLogicalDevice(){
    //get struct of all queues to be interfaced with
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<unsigned int> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()};

    float queuePriority = 1.f;

    for(unsigned int queueFamily : uniqueQueueFamilies){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    //create actual logical device
    VkPhysicalDeviceFeatures deviceFeatures{}; //does nothing now, can fill with features later

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature {};
    dynamicRenderingFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRenderingFeature.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures{};
    meshShaderFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
    meshShaderFeatures.meshShader = VK_TRUE;
    meshShaderFeatures.taskShader = VK_TRUE;
    meshShaderFeatures.pNext = &dynamicRenderingFeature;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &meshShaderFeatures;

    //add queues
    createInfo.queueCreateInfoCount = static_cast<unsigned int>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    //add extensions (swapchain)
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<unsigned int>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    //add debugging
    if(enableValidationLayers){
        createInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS){
        throw std::runtime_error("Failed to create logical device");
    }

    //store logical queues
    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
    vkGetDeviceQueue(logicalDevice, indices.transferFamily.value(), 0, &transferQueue);
}

void Renderer::createSwapchain(){
    SwapchainSupportDetails swapchainSupport = querySwapchainSupport( physicalDevice );

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat( swapchainSupport.formats ); //list of formats - pixel format, color space
    VkPresentModeKHR presentMode = chooseSwapPresentMode( swapchainSupport.presentModes ); //list of present modes - how images are presented, (vsync + FIFO/Mailbox/Immediate frame queueing)
    VkExtent2D extent = chooseSwapExtent( swapchainSupport.capabilities ); //struct containing size of window, resolution of images

    unsigned int imageCount = swapchainSupport.capabilities.minImageCount + 1; //number of images in swapchain, at least 2 for double buffering

    //clamp imageCount to maxImageCount if there is a max
    if ( swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount )
    {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{}; //information for creating swapchain object
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; //how many layers in each image, usually 1 for standard use, but can be more for VR, cube maps, and more
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //stores color data, could also store depth data, among other things

    QueueFamilyIndices indices = findQueueFamilies( physicalDevice );
    unsigned int queueFamilyIndices[] = { indices.graphicsFamily.value( ), indices.presentFamily.value( ), indices.transferFamily.value() };

    if ( indices.graphicsFamily != indices.presentFamily )
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; //image owned by many queue families, slower, but no manual switching

        //only needed for concurrent sharing mode, provides information about queues so Vulkan knows which queues can access the image
        createInfo.queueFamilyIndexCount = 3;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //image owned by one queue family at a time, faster, but more overhead for switching
    }

    createInfo.preTransform = swapchainSupport.capabilities.currentTransform; //dont transform image before displaying
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode; 
    createInfo.clipped = VK_TRUE;   //dont render pixels if offscreen

    if ( vkCreateSwapchainKHR( logicalDevice, &createInfo, nullptr, &swapchain ) != VK_SUCCESS )
    {
        throw std::runtime_error( "failed to create swap chain!" );
    }

    //store images in swapchainImages so you can render to and present those images later
    vkGetSwapchainImagesKHR( logicalDevice, swapchain, &imageCount, nullptr );
    swapchainImages.resize( imageCount );
    vkGetSwapchainImagesKHR( logicalDevice, swapchain, &imageCount, swapchainImages.data( ) );

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;
}

void Renderer::createImageViews(){
    swapchainImageViews.resize(swapchainImages.size());
    for(unsigned long long i = 0; i < swapchainImages.size(); i++){
        swapchainImageViews[i] = createImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void Renderer::createShaderModules(){
    std::vector<char> taskShaderCode = readFile(taskShaderFilename);
    std::vector<char> meshShaderCode = readFile(meshShaderFilename);
    std::vector<char> fragShaderCode = readFile(fragShaderFilename);

    taskShaderModule = createShaderModule(taskShaderCode);
    meshShaderModule = createShaderModule(meshShaderCode);
    fragShaderModule = createShaderModule(fragShaderCode);
}

void Renderer::createCommandPools(){
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo graphicsPoolInfo{};
    graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    graphicsPoolInfo.queueFamilyIndex = indices.graphicsFamily.value();
    graphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    if(vkCreateCommandPool(logicalDevice, &graphicsPoolInfo, nullptr, &graphicsCommandPool) != VK_SUCCESS){
        throw std::runtime_error("Failed to create graphics command pool");
    }

    VkCommandPoolCreateInfo transferPoolInfo{};
    transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    transferPoolInfo.queueFamilyIndex = indices.transferFamily.value();
    
    if(vkCreateCommandPool(logicalDevice, &transferPoolInfo, nullptr, &transferCommandPool) != VK_SUCCESS){
        throw std::runtime_error("Failed to create transfer command pool");
    }
}

void Renderer::createUniformBuffers(){
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            uniformBuffers[i], uniformBuffersMemory[i]);

        vkMapMemory(logicalDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void Renderer::createShaderStorageBufferObjects() {

    VkDeviceSize bufferSize = scene->getVertexBufferSize();

    if (bufferSize == 0) {
        throw std::runtime_error("No vertex data available.");
    }

    VkBuffer vertexStagingBuffer;
    VkDeviceMemory vertexStagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexStagingBuffer, vertexStagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, vertexStagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, scene->getVertexData().data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, vertexStagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexSSBO, vertexSSBOMemory);
    copyBuffer(vertexStagingBuffer, vertexSSBO, bufferSize);

    vkDestroyBuffer(logicalDevice, vertexStagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, vertexStagingBufferMemory, nullptr);

    bufferSize = scene->getInstanceBufferSize();

    if (bufferSize == 0) {
        throw std::runtime_error("No instance data available.");
    }

    VkBuffer instanceStagingBuffer;
    VkDeviceMemory instanceStagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, instanceStagingBuffer, instanceStagingBufferMemory);

    vkMapMemory(logicalDevice, instanceStagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, scene->getInstanceData().data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, instanceStagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instanceSSBO, instanceSSBOMemory);
    copyBuffer(instanceStagingBuffer, instanceSSBO, bufferSize);

    vkDestroyBuffer(logicalDevice, instanceStagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, instanceStagingBufferMemory, nullptr);
}

void Renderer::createDescriptorSetLayout(){

    std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};

    layoutBindings[0].binding = 0;                                           //binding location in shader
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[0].descriptorCount = 1;                                   //could be an array of ubos
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_TASK_BIT_EXT;               //which shaders will use this ubo
    layoutBindings[0].pImmutableSamplers = nullptr;

    layoutBindings[1].binding = 1;                                           //binding location in shader
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings[1].descriptorCount = 1;                                   //could be an array of ubos
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_TASK_BIT_EXT;               //which shaders will use this ubo
    layoutBindings[1].pImmutableSamplers = nullptr;

    layoutBindings[2].binding = 2;                                           //binding location in shader
    layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings[2].descriptorCount = 1;                                   //could be an array of ubos
    layoutBindings[2].stageFlags = VK_SHADER_STAGE_TASK_BIT_EXT;               //which shaders will use this ubo
    layoutBindings[2].pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 3;
    layoutInfo.pBindings = layoutBindings.data();

    if(vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS){
        throw std::runtime_error("Failed to create descriptor set layout");
    }
}

void Renderer::createDescriptorPool(){
    //can have multiple pool sizes, one for each type of descriptor
    //used to find out how much total memory will be needed for the pool for all descriptors
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = 2 * static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    //creates the descriptor pool with all the specified pool sizes.
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT); //need this because each descriptor set comes with its own memory needs outside of its contents
    
    if(vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS){
        throw std::runtime_error("Failed to create descriptor pool");
    }
}

void Renderer::createDescriptorSets(){
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if(vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate descriptor sets");
    }

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){

        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        descriptorWrites[0].pImageInfo = nullptr;
        descriptorWrites[0].pTexelBufferView = nullptr;

        VkDescriptorBufferInfo vertexSSBOInfo{};
        vertexSSBOInfo.buffer = vertexSSBO;
        vertexSSBOInfo.offset = 0;
        vertexSSBOInfo.range = scene->getVertexBufferSize();

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &vertexSSBOInfo;
        descriptorWrites[1].pImageInfo = nullptr;
        descriptorWrites[1].pTexelBufferView = nullptr;

        VkDescriptorBufferInfo instanceSSBOInfo{};
        instanceSSBOInfo.buffer = instanceSSBO;
        instanceSSBOInfo.offset = 0;
        instanceSSBOInfo.range = scene->getInstanceBufferSize();

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = descriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &instanceSSBOInfo;
        descriptorWrites[2].pImageInfo = nullptr;
        descriptorWrites[2].pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(logicalDevice, 3, descriptorWrites.data(), 0, nullptr);

    }
}

void Renderer::createPipeline(){
    VkPipelineShaderStageCreateInfo taskShaderStageInfo{};
    taskShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    taskShaderStageInfo.stage = VK_SHADER_STAGE_TASK_BIT_EXT;
    taskShaderStageInfo.module = taskShaderModule;
    taskShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo meshShaderStageInfo{};
    meshShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    meshShaderStageInfo.stage = VK_SHADER_STAGE_MESH_BIT_EXT;
    meshShaderStageInfo.module = meshShaderModule;
    meshShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {taskShaderStageInfo, meshShaderStageInfo, fragShaderStageInfo};

    //map between pixels and normalized coordinates
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchainExtent.width;
    viewport.height = (float)swapchainExtent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    //get rid of everything offscreen
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchainExtent;

    //contains all viewports and scissors
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    //determines fragments/pixels before frag shader
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    //for anti-aliasing
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    //how are new colors blended with colors already on framebuffer
    //faster than bouncing between framebuffers for simple post-processing effects
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;                 //combine colors with bitwise operations between new and old colors
    colorBlending.logicOp = VK_LOGIC_OP_COPY;               //replace old color with new
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    
    //fixed color for blending effects (tint, overlay, etc)
    colorBlending.blendConstants[0] = 0.f;  //r
    colorBlending.blendConstants[1] = 0.f;  //g
    colorBlending.blendConstants[2] = 0.f;  //b
    colorBlending.blendConstants[3] = 0.f;  //a

    VkPipelineRenderingCreateInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats = &swapchainImageFormat;
    renderingInfo.depthAttachmentFormat = depthFormat;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    if(vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
        throw std::runtime_error("Failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &renderingInfo;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.stageCount = 3;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = NULL;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    
    if(vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS){
        throw std::runtime_error("Failed to create graphics pipeline.");
    }

    vkDestroyShaderModule(logicalDevice, taskShaderModule, nullptr);
    vkDestroyShaderModule(logicalDevice, meshShaderModule, nullptr);
    vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
}

void Renderer::createCommandBuffers(){
    commandBuffers.resize(swapchainImages.size());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = graphicsCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (unsigned int)commandBuffers.size();
    
    if(vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate command buffers");
    }
}

void Renderer::createSyncObjects(){
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(swapchainImages.size());
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(unsigned long long i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        if(vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS){
            throw std::runtime_error("Failed to create synchronization objects for a frame");
        }
    }

    for (int i = 0; i < swapchainImages.size(); i++) {
        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create synchronization objects for a frame");
        }
    }
}

void Renderer::createDepthResources(){
    depthFormat = findDepthFormat();
    depthImages.resize(swapchainImages.size());
    depthImagesMemory.resize(swapchainImages.size());
    depthImageViews.resize(swapchainImages.size());

    for(int i = 0; i < swapchainImages.size(); i++){
        createImage(swapchainExtent.width, swapchainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i], depthImagesMemory[i]);
        depthImageViews[i] = createImageView(depthImages[i], depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        transitionImageLayout(commandBuffer, depthImages[i], depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
        endSingleTimeCommands(commandBuffer);
    }
}

void Renderer::recordCommandBuffers(int i){
    vkResetCommandBuffer(commandBuffers[i],VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS){
        throw std::runtime_error("Failed to begin recording command buffer");
    }

    transitionImageLayout(commandBuffers[i], swapchainImages[i], swapchainImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    transitionImageLayout(commandBuffers[i], depthImages[i], depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfoKHR colorAttachmentInfo{};
    colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    colorAttachmentInfo.imageView = swapchainImageViews[i];
    colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    VkClearValue clearColor = {{{0.f, 0.f, 0.f, 1.f}}};
    colorAttachmentInfo.clearValue = clearColor;

    VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
    depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    depthAttachmentInfo.imageView = depthImageViews[i];
    depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentInfo.clearValue = {{1.0f, 0}};

    VkRenderingInfoKHR renderInfo{};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    renderInfo.renderArea.offset = {0, 0};
    renderInfo.renderArea.extent = swapchainExtent;
    renderInfo.layerCount = 1;
    renderInfo.colorAttachmentCount = 1;
    renderInfo.pColorAttachments = &colorAttachmentInfo;
    renderInfo.pDepthAttachment = &depthAttachmentInfo;

    vkCmdBeginRendering(commandBuffers[i], &renderInfo);
    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    
    VkDeviceSize offsets[] = {0};

    vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
    vkCmdDrawMeshTasksEXT(commandBuffers[i], 40, 1, 1);
    vkCmdEndRendering(commandBuffers[i]);
    transitionImageLayout(commandBuffers[i], swapchainImages[i], swapchainImageFormat, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
        throw std::runtime_error("Failed to record command buffer");
    }
}

void Renderer::updateUniformBuffer(uint32_t currentImage){

    UniformBufferObject ubo{};
    ubo.view = scene->getCamera().getViewMatrix();
    ubo.viewTranslate = -scene->getCamera().getPosition();
    //glm::vec4 viewTranslate = -scene->getCamera().getPosition();
    //std::cout << "viewTranslate: " << viewTranslate.x << " " << viewTranslate.y << " " << viewTranslate.z << " " << viewTranslate.w << std::endl;
    ubo.proj = glm::perspective(glm::radians(45.0f), swapchainExtent.width / (float) swapchainExtent.height, 0.1f, 100.f);
    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void Renderer::drawFrame(){
    //currentFrame cycles through all frames in flight and gets back to the same one before rendering is finished
    //waits for all rendering to finish before continuing
    vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);


    //gets an image from the swapchain that is not being used to present
    //signals imageAvailableSemaphores once a result is returned
    unsigned int imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    

    if(result == VK_ERROR_OUT_OF_DATE_KHR){ //swapchain not compatible with surface, must recreate swapchain
        recreateSwapchain();
        return;
    } else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {    //image not acquired
        throw std::runtime_error("Failed to acquire swapchain image");
    }   

    recordCommandBuffers(imageIndex);


    updateUniformBuffer(currentFrame);
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    

    //waits for imageAvailableSemaphores to be signaled(from vkAcquireNextImageKHR) before writing to color attachment output bit
    //waits for presentFinishedSemaphores to be signaled to show image is not currently being presented.
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    //once finished, signal renderFinishedSemaphores to allow for presenting
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[imageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);
    //submits pCommandBuffers to graphics queue
    //will signal/unblock current fence when command buffers complete

    VkResult submitResult = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);
    if(submitResult != VK_SUCCESS){
        std::cout << "vkQueueSubmit result: " << submitResult << std::endl;
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    //present will wait for renderFinishedSemaphores (graphics queue finished)
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    //present to image acquired above from swapchain
    VkSwapchainKHR swapchains[] = {swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    
    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    // SC is incompatible, must recreate  || SC not optimal, should recreate || framebuffer was resized
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized){
        framebufferResized = false;
        recreateSwapchain();
    } else if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire swapchain image");
    }   

    //increments currentFrame to next frame (or first if on last)
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::initialize(GLFWwindow* window, Scene* scene){
    this->window = window;
    this->scene = scene;
    srand(time(0));

    createInstance();
    createDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    
    createSwapchain();
    createImageViews();

    createShaderModules();
    createCommandPools();

    createDepthResources();

    createUniformBuffers();
    createShaderStorageBufferObjects();

    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();

    createPipeline();
    createCommandBuffers();
    createSyncObjects();
}

