#pragma once

#include "lava.h"
#include "SwapChain.h"

namespace lava
{
  class Device
  {
  public:
    std::function<void(VkCommandBuffer)> bindingBufferFunction = nullptr;

    struct QueueFamilyIndices
    {
      int graphicsFamily = -1;
      int presentFamily = -1;
      bool isComplete() { return graphicsFamily >= 0 && presentFamily >= 0; }
    };
  public:
    Device() = default;
    virtual ~Device() = default;

    Device& create();
    Device& destroy();
    Device& enableValidation(bool _enable) { enableValidationLayers = _enable; return *this; }
    Device& windowHandle(void* _handle) { hwnd = _handle; return *this; }
    Device& targetExtend(VkExtent2D _extend) { windowSize = _extend; return *this; }
    const VkExtent2D& targetExtend() const { return windowSize; }
    const SwapChain& swapChain() const { return swpChain; }

    VkSurfaceCapabilitiesKHR surfaceCapabilities() const { return swapChainSupportDetails.capabilities; }
    const std::vector<VkSurfaceFormatKHR>& surfaceFormats() const { return swapChainSupportDetails.formats; }
    const std::vector<VkPresentModeKHR>& presentModes() const { return swapChainSupportDetails.presentModes; }

    QueueFamilyIndices physicalDeviceQueueFamilyIndices() const { return queueFamilyIndices; }
    VkSurfaceKHR surfaceKHR() const { return surface; }

    VkDevice vulkanDevice() const { return device; }
    VkPhysicalDevice vulkanPhysicalDevice() const { return physicalDevice; }
    
    uint32_t findMemoryType(uint32_t  typeFilter, VkMemoryPropertyFlags  properties);
    uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr);

    void createBuffer(const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer)
    {
      vkCall(vkCreateBuffer(device, pCreateInfo, pAllocator, pBuffer));
    }

    void allocateMemory(const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory *pMemory)
    {
      vkCall(vkAllocateMemory(device, pAllocateInfo, pAllocator, pMemory));
    }

    void bindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset)
    {
      vkCall(vkBindBufferMemory(device, buffer, memory, memoryOffset));
    }

    VkCommandBuffer beginSingleTimeCommands()
    {
      VkCommandBufferAllocateInfo allocInfo = {};
      allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      allocInfo.commandPool = commandPool;
      allocInfo.commandBufferCount = 1;

      VkCommandBuffer commandBuffer;
      vkCall(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer));

      VkCommandBufferBeginInfo beginInfo = {};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

      vkCall(vkBeginCommandBuffer(commandBuffer, &beginInfo));

      return commandBuffer;
    }

    void endSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
      vkEndCommandBuffer(commandBuffer);

      VkSubmitInfo submitInfo = {};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &commandBuffer;

      vkCall(vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
      vkCall(vkQueueWaitIdle(graphicsQueue));

      vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
      VkCommandBuffer commandBuffer = beginSingleTimeCommands();

      VkBufferCopy copyRegion = {};
      copyRegion.size = size;
      vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

      endSingleTimeCommands(commandBuffer);
    }

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
      VkCommandBuffer commandBuffer = beginSingleTimeCommands();

      VkImageMemoryBarrier barrier = {};
      barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.oldLayout = oldLayout;
      barrier.newLayout = newLayout;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = image;

      if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
      {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
          barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
      }
      else
      {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      }

      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;

      VkPipelineStageFlags sourceStage;
      VkPipelineStageFlags destinationStage;

      if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      }
      else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      }
      else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      }
      else {
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

      endSingleTimeCommands(commandBuffer);
    }

    VkBool32 getSupportedDepthFormat(VkFormat *depthFormat)
    {
      // Since all depth formats may be optional, we need to find a suitable depth format to use
      // Start with the highest precision packed format
      std::vector<VkFormat> depthFormats = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
      };

      for (auto& format : depthFormats)
      {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
        // Format must support depth stencil attachment for optimal tiling
        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
          *depthFormat = format;
          return true;
        }
      }

      return false;
    }

    void drawFrame()
    {
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swpChain.swapchainKHR(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            //recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo);

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swpChain.renderPass();
        renderPassInfo.framebuffer = swpChain.frameBuffers()[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swpChain.extent();

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = { 0.25f, 0.25f, 0.25f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        bindingBufferFunction(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);

        vkCall(vkEndCommandBuffer(commandBuffers[imageIndex]));

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkCall(vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swpChain.swapchainKHR() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            //recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        vkCall(vkQueueWaitIdle(presentQueue));
    }
    
  protected:
    
    struct SwapChainSupportDetails
    {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR>   presentModes;
    };

    VkInstance       instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice         device = VK_NULL_HANDLE;
    VkSurfaceKHR     surface = VK_NULL_HANDLE;
    VkQueue          graphicsQueue;
    VkQueue          presentQueue;
    VkCommandPool    commandPool;
    void*            hwnd = nullptr;
    VkExtent2D       windowSize;
    bool             enableValidationLayers = false;

    SwapChainSupportDetails swapChainSupportDetails;
    QueueFamilyIndices queueFamilyIndices;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    SwapChain        swpChain;
    std::vector<VkCommandBuffer> commandBuffers;


    void createInstance();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();
    void createSemaphores();
    void createCommandBuffers();

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    // Functions to find the best physical device
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  };

}
