//
// Created by taco on 7/7/24.
//

#ifndef EJETSYSTEMS_VULKANUTILS_H
#define EJETSYSTEMS_VULKANUTILS_H


#include <cstdio>
#include <cstdlib>
#include <utility>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include "Fonts.h"
#include "backends/imgui_impl_vulkan.h"

namespace E170Systems::Renderer {

    using namespace E170Systems::Renderer;
    static VkAllocationCallbacks *g_Allocator = nullptr;
    static VkInstance g_Instance = VK_NULL_HANDLE;
    static VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
    static VkDevice g_Device = VK_NULL_HANDLE;
    static uint32_t g_QueueFamily = (uint32_t) -1;
    static VkQueue g_Queue = VK_NULL_HANDLE;
    static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
    static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
    static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;

    static ImGui_ImplVulkanH_Window g_MainWindowData;
    static int g_MinImageCount = 2;
    static bool g_SwapChainRebuild = false;

    static void glfw_error_callback(int error, const char *description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    static void check_vk_result(VkResult err) {
        if (err == 0)
            return;
        fprintf(stderr, "[Vulkan] error: VkResult = %d\n", err);
        if (err < 0) {}
        abort();
    }

    static bool IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties, const char *extension) {
        for (const VkExtensionProperties &p: properties) {
            if (strcmp(p.extensionName, extension) == 0) {
                return true;
            }
        }
        return false;
    }

    static VkPhysicalDevice SetupVulkan_SelectPhysicalDevice() {
        uint32_t gpu_count;
        VkResult err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);
        check_vk_result(err);
        IM_ASSERT(gpu_count > 0);

        ImVector<VkPhysicalDevice> gpus;
        gpus.resize(gpu_count);
        err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.Data);
        check_vk_result(err);

        for (VkPhysicalDevice device: gpus) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                return device; // we will go off gpu 1 (first actual gpu)
        }

        if (gpu_count > 0) {
            return gpus[0];
        }
        return VK_NULL_HANDLE;
    }

    static void SetupVulkan(ImVector<const char *> instance_expressions) {
        VkResult err;

        {
            VkInstanceCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

            uint32_t propertiesCount;
            ImVector<VkExtensionProperties> properties;
            vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr);
            properties.resize(propertiesCount);
            err = vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, properties.Data);
            check_vk_result(err);

            if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
                instance_expressions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);


#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
            if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
                instance_expressions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
                createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
            }
#endif


            createInfo.enabledExtensionCount = (uint32_t) instance_expressions.Size;
            createInfo.ppEnabledExtensionNames = instance_expressions.Data;
            err = vkCreateInstance(&createInfo, g_Allocator, &g_Instance);
            check_vk_result(err);


        }
        g_PhysicalDevice = SetupVulkan_SelectPhysicalDevice();

        {
            uint32_t count;
            vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
            VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *) malloc(
                    sizeof(VkQueueFamilyProperties) * count);
            vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);

            for (uint32_t i = 0; i < count; ++i) {
                if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {}
                g_QueueFamily = i;
                break;
            }
            free(queues);
            IM_ASSERT(g_QueueFamily != (uint32_t) -1);

        }

        {
            ImVector<const char *> device_extensions;
            device_extensions.push_back("VK_KHR_swapchain");

            uint32_t properties_count;
            ImVector<VkExtensionProperties> properties;
            vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count, nullptr);
            properties.resize(properties_count);
            vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count, properties.Data);

#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
            if(IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
                device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif
            const float queue_priority[] = {1.0f};
            VkDeviceQueueCreateInfo queue_info[1] = {};
            queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info[0].queueFamilyIndex = g_QueueFamily;
            queue_info[0].queueCount = 1;
            queue_info[0].pQueuePriorities = queue_priority;
            VkDeviceCreateInfo create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
            create_info.pQueueCreateInfos = queue_info;
            create_info.enabledExtensionCount = (uint32_t) device_extensions.Size;
            create_info.ppEnabledExtensionNames = device_extensions.Data;
            err = vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device);
            check_vk_result(err);
            vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
        }

        {
            VkDescriptorPoolSize pool_sizes[] =
                    {
                            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
                    };
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1;
            pool_info.poolSizeCount = (uint32_t) IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes = pool_sizes;
            err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
            check_vk_result(err);
        }
    }

    static void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VkSurfaceKHR surface, int width, int height) {
        wd->Surface = surface;

        VkBool32 res;
        vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
        if (res != VK_TRUE) {
            fprintf(stderr, "Error no WSI support on physical device 0\n");
            exit(-1);
        }

        const VkFormat requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
                                                      VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
        const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface,
                                                                  requestSurfaceImageFormat,
                                                                  (size_t) IM_ARRAYSIZE(requestSurfaceImageFormat),
                                                                  requestSurfaceColorSpace);

#ifdef APP_USE_UNLIMITED_FRAME_RATE
        VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
        VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
        wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0],
                                                              IM_ARRAYSIZE(present_modes));

        IM_ASSERT(g_MinImageCount >= 2);
        ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator,
                                               width, height, g_MinImageCount);

    }

    static void CleanupVulkan() {
        vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
        vkDestroyDevice(g_Device, g_Allocator);
        vkDestroyInstance(g_Instance, g_Allocator);
    }

    static void CleanupVulkanWindow() {
        ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
    }

    static void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data) {
        VkResult err;

        VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE,
                                    &wd->FrameIndex);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            g_SwapChainRebuild = true;
            return;
        }
        check_vk_result(err);

        ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
        {
            err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE,
                                  UINT64_MAX);    // wait indefinitely instead of periodically checking
            check_vk_result(err);

            err = vkResetFences(g_Device, 1, &fd->Fence);
            check_vk_result(err);
        }
        {
            err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
            check_vk_result(err);
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
            check_vk_result(err);
        }
        {
            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass = wd->RenderPass;
            info.framebuffer = fd->Framebuffer;
            info.renderArea.extent.width = wd->Width;
            info.renderArea.extent.height = wd->Height;
            info.clearValueCount = 1;
            info.pClearValues = &wd->ClearValue;
            vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

        vkCmdEndRenderPass(fd->CommandBuffer);
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount = 1;
            info.pWaitSemaphores = &image_acquired_semaphore;
            info.pWaitDstStageMask = &wait_stage;
            info.commandBufferCount = 1;
            info.pCommandBuffers = &fd->CommandBuffer;
            info.signalSemaphoreCount = 1;
            info.pSignalSemaphores = &render_complete_semaphore;

            err = vkEndCommandBuffer(fd->CommandBuffer);
            check_vk_result(err);
            err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
            check_vk_result(err);
        }
    }

    static void FramePresent(ImGui_ImplVulkanH_Window *wd) {
        if (g_SwapChainRebuild)
            return;
        VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &render_complete_semaphore;
        info.swapchainCount = 1;
        info.pSwapchains = &wd->Swapchain;
        info.pImageIndices = &wd->FrameIndex;
        VkResult err = vkQueuePresentKHR(g_Queue, &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            g_SwapChainRebuild = true;
            return;
        }
        check_vk_result(err);
        wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount;
    }

    static void InitFrameBuffer(GLFWwindow *window) {
        int fb_width, fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        if (fb_width > 0 && fb_height > 0 &&
            (g_SwapChainRebuild || g_MainWindowData.Width != fb_width || g_MainWindowData.Height != fb_height)) {
            ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData,
                                                   g_QueueFamily, g_Allocator, fb_width, fb_height, g_MinImageCount);
            g_MainWindowData.FrameIndex = 0;
            g_SwapChainRebuild = false;
        }
    }

    struct ImGuiInitData {
        ImGuiIO io;
        ImGui_ImplVulkanH_Window wd;
    };

    static ImGuiInitData InitImGui(GLFWwindow *window) {
        ImVector<const char *> extensions;
        uint32_t extensions_count = 0;
        const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
        for (uint32_t i = 0; i < extensions_count; i++)
            extensions.push_back(glfw_extensions[i]);
        SetupVulkan(extensions);

        VkSurfaceKHR surface;
        VkResult err = glfwCreateWindowSurface(g_Instance, window, g_Allocator, &surface);
        check_vk_result(err);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;
        SetupVulkanWindow(wd, surface, w, h);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = g_Instance;
        init_info.PhysicalDevice = g_PhysicalDevice;
        init_info.Device = g_Device;
        init_info.QueueFamily = g_QueueFamily;
        init_info.Queue = g_Queue;
        init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = g_DescriptorPool;
        init_info.RenderPass = wd->RenderPass;
        init_info.Subpass = 0;
        init_info.MinImageCount = g_MinImageCount;
        init_info.ImageCount = wd->ImageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = g_Allocator;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info);


        ImGuiInitData initData;
        initData.io = io;
        initData.wd = *wd;

        return initData;
    }

    static void DestroyApp(GLFWwindow *window) {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        CleanupVulkanWindow();
        CleanupVulkan();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

}


#endif //EJETSYSTEMS_VULKANUTILS_H
