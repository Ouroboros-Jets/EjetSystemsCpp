#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#ifdef INFINITY_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif INFINITY_WAYLAND
#define GLFW_EXPOSE_NATIVE_WAYLAND
#elif INFINITY_X11
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include "Application.hpp"
#include "Backend/ImGuiTheme/ImGuiTheme.hpp"
#include "Backend/UiHelpers/UiHelpers.hpp"
#include "Backend/VulkanManager/VulkanManager.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui_internal.h"

#include <algorithm>
#include <corecrt_io.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>

#include <vulkan/vulkan.h>
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "stb_image/stb_image.h"


using namespace Ouroboros::Vulkan;

extern bool g_ApplicationRunning;

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif


static std::unordered_map<std::string, ImFont *> s_Fonts;

static Ouroboros::Application *s_Instance = nullptr;

constexpr int FPS_CAP = 144;
constexpr double FRAME_DURATION = 1.0 / FPS_CAP;

GLFWwindow *Ouroboros::Application::s_WindowHandle = nullptr;


#include "Assets/Fonts/Roboto-Bold.h"
#include "Assets/Fonts/Roboto-Italic.h"
#include "Assets/Fonts/Roboto-Regular.h"
#include "Assets/Images/ouroborosIcon.h"
#include "Assets/Images/overhead.h"
#include "Assets/Images/pedestal.h"

namespace Ouroboros {
    Application::Application(ApplicationSpecifications applicationSpecification) : m_Specification(std::move(applicationSpecification)) {
        s_Instance = this;
        Init();
    }

    Application::~Application() {
        Shutdown();
        s_Instance = nullptr;
    }

    std::optional<Application *> Application::Get() { return s_Instance; }

    void Application::Init() {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            std::cerr << "could not init glfw\n";
            return;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *videoMode = glfwGetVideoMode(primaryMonitor);

        int monitorX, monitorY;
        glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);


        m_WindowHandle = glfwCreateWindow(static_cast<int>(m_Specification.Width), static_cast<int>(m_Specification.Height), m_Specification.Name.c_str(), nullptr, nullptr);

        s_WindowHandle = m_WindowHandle;


        glfwSetWindowSizeLimits(m_WindowHandle, static_cast<int>(m_Specification.MinWidth), static_cast<int>(m_Specification.MinHeight), static_cast<int>(m_Specification.MaxWidth),
                                static_cast<int>(m_Specification.MaxHeight));

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        if (m_Specification.CenterWindow) {
            glfwSetWindowPos(m_WindowHandle, static_cast<int>(monitorX + (videoMode->width - m_Specification.Width) / 2),
                             static_cast<int>(monitorY + (videoMode->height - m_Specification.Height) / 2));
        }

        SetWindowIcon(m_WindowHandle, g_OuroborosIcon, sizeof(g_OuroborosIcon));

        glfwShowWindow(m_WindowHandle);

        if (!glfwVulkanSupported()) {
            std::cerr << "Vulkan is not supported on this device\n";
            return;
        }

        glfwSetWindowUserPointer(m_WindowHandle, this);
        glfwSetTitlebarHitTestCallback(m_WindowHandle, [](GLFWwindow *window, int x, int y, int *hit) {
            const auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
            *hit = app->IsTitleBarHovered();
        });

        uint32_t extensions_count = 0;
        const char **extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
        SetupVulkan(extensions, extensions_count);

        VkSurfaceKHR surface;
        VkResult err = glfwCreateWindowSurface(g_Instance, m_WindowHandle, g_Allocator, &surface);
        inf_check_vk_result(err);

        int w, h;
        glfwGetFramebufferSize(m_WindowHandle, &w, &h);
        ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;
        SetupVulkanWindow(wd, surface, w, h);

        s_AllocatedCommandBuffers.resize(wd->ImageCount);
        s_ResourceFreeQueue.resize(wd->ImageCount);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        SetDarkTheme();


        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(8.0f, 6.0f);
        style.ItemSpacing = ImVec2(6.0f, 6.0f);
        style.ChildRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.FrameRounding = 6.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 12.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = g_Instance;
        init_info.PhysicalDevice = g_PhysicalDevice;
        init_info.Device = g_Device;
        init_info.QueueFamily = g_QueueFamily;
        init_info.Queue = g_Queue;
        init_info.PipelineCache = g_PipelineCache;
        init_info.DescriptorPool = g_DescriptorPool;
        init_info.Subpass = 0;
        init_info.MinImageCount = g_MinImageCount;
        init_info.ImageCount = wd->ImageCount;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = g_Allocator;
        init_info.CheckVkResultFn = inf_check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);


        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        ImFont *robotoFont = io.Fonts->AddFontFromMemoryTTF(g_RobotoRegular, sizeof(g_RobotoRegular), 20.0f, &fontConfig);
        s_Fonts["Default"] = robotoFont;
        s_Fonts["Bold"] = io.Fonts->AddFontFromMemoryTTF(g_RobotoBold, sizeof(g_RobotoBold), 20.0f, &fontConfig);
        s_Fonts["Italic"] = io.Fonts->AddFontFromMemoryTTF(g_RobotoItalic, sizeof(g_RobotoItalic), 20.0f, &fontConfig);
        s_Fonts["DefaultLarge"] = io.Fonts->AddFontFromMemoryTTF(g_RobotoRegular, sizeof(g_RobotoRegular), 40.0f, &fontConfig);

        io.FontDefault = robotoFont;
        {
            VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
            VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

            err = vkResetCommandPool(g_Device, command_pool, 0);
            inf_check_vk_result(err);
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(command_buffer, &begin_info);
            inf_check_vk_result(err);

            ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

            VkSubmitInfo end_info = {};
            end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            end_info.commandBufferCount = 1;
            end_info.pCommandBuffers = &command_buffer;
            err = vkEndCommandBuffer(command_buffer);
            inf_check_vk_result(err);
            err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
            inf_check_vk_result(err);

            err = vkDeviceWaitIdle(g_Device);
            inf_check_vk_result(err);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    }

    void Application::Shutdown() {
        for (const auto &layer: m_LayerStack)
            layer->OnDetach();

        m_LayerStack.clear();
        m_AppHeaderIcon.reset();
        m_IconClose.reset();
        m_IconMinimize.reset();
        m_IconMaximize.reset();
        m_IconRestore.reset();

        const VkResult err = vkDeviceWaitIdle(g_Device);
        inf_check_vk_result(err);

        for (auto &queue: s_ResourceFreeQueue) {
            for (auto &func: queue)
                func();
        }
        s_ResourceFreeQueue.clear();

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        CleanupVulkanWindow();
        CleanupVulkan();

        glfwDestroyWindow(m_WindowHandle);
        glfwTerminate();

        g_ApplicationRunning = false;
    }


    void Application::SetWindowIcon(GLFWwindow *window, const unsigned char *data, const int size) {
        GLFWimage images[1];
        images[0].pixels = stbi_load_from_memory(data, size, &images[0].width, &images[0].height, nullptr, 4);

        if (images[0].pixels == nullptr) {
            fprintf(stderr, "Failed to load image from memory\n");
            return;
        }
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
    }

    void Application::Run() {
        m_Running = true;

        ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;
        const auto clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        ImGuiIO &io = ImGui::GetIO();

        io.IniFilename = nullptr;


        // Main loop
        while (!glfwWindowShouldClose(m_WindowHandle) && m_Running) {
            const double startTime = glfwGetTime();
            glfwPollEvents();
            {
                std::scoped_lock<std::mutex> lock(m_EventQueueMutex);

                while (!m_EventQueue.empty()) {
                    auto &func = m_EventQueue.front();
                    func();
                    m_EventQueue.pop();
                }
            }

            for (const auto &layer: m_LayerStack)
                layer->OnUpdate(m_TimeStep);


            if (g_SwapChainRebuild) {
                int width, height;
                glfwGetFramebufferSize(m_WindowHandle, &width, &height);
                if (width > 0 && height > 0) {
                    ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
                    ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
                    g_MainWindowData.FrameIndex = 0;

                    s_AllocatedCommandBuffers.clear();
                    s_AllocatedCommandBuffers.resize(g_MainWindowData.ImageCount);

                    g_SwapChainRebuild = false;
                }
            }

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

                const ImGuiViewport *viewport = ImGui::GetMainViewport();
                const ImVec2 windowPos = viewport->Pos;
                ImGui::SetNextWindowPos(ImVec2(windowPos.x - 1, windowPos.y));
                ImGui::SetNextWindowSize(ImVec2(viewport->Size.x + 1, viewport->Size.y));
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


                ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));
                for (const auto &layer: m_LayerStack)
                    layer->OnUIRender();

                ImGui::PopStyleVar(2);
                ImGui::PopStyleVar(3);

                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            ImDrawData *main_draw_data = ImGui::GetDrawData();
            const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
            wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
            wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
            wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
            wd->ClearValue.color.float32[3] = clear_color.w;
            if (!main_is_minimized)
                FrameRender(wd, main_draw_data);

            // Update and Render additional Platform Windows
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }

            if (!main_is_minimized)
                FramePresent(wd);
            else {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(5ms);
            }


            const float time = GetTime();
            m_FrameTime = time - m_LastFrameTime;
#ifdef INFINITY_WINDOWS
            m_TimeStep = min(m_FrameTime, 0.0333f);
#else
            m_TimeStep = std::min(m_FrameTime, 0.0333f);
#endif
            m_LastFrameTime = time;

            const double endTime = glfwGetTime();

            if (const double frameTime = endTime - startTime; frameTime < FRAME_DURATION) {
                std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_DURATION - frameTime));
            }
        }
    }

    void Application::Close() { m_Running = false; }

    bool Application::IsMaximized() const { return static_cast<bool>(glfwGetWindowAttrib(m_WindowHandle, GLFW_MAXIMIZED)); }

    float Application::GetTime() { return static_cast<float>(glfwGetTime()); }

    std::optional<VkInstance> Application::GetInstance() {
        if (g_Instance != nullptr) {
            return g_Instance;
        }
        return std::nullopt;
    }

    std::optional<VkPhysicalDevice> Application::GetPhysicalDevice() {
        if (g_PhysicalDevice != nullptr) {
            return g_PhysicalDevice;
        }
        return std::nullopt;
    }

    std::optional<VkDevice> Application::GetDevice() {
        if (g_Device != nullptr) {
            return g_Device;
        }
        return std::nullopt;
    }

    VkCommandBuffer Application::GetCommandBuffer(bool begin) {
        const ImGui_ImplVulkanH_Window *wd = &g_MainWindowData;
        VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;

        VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
        cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdBufAllocateInfo.commandPool = command_pool;
        cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBufAllocateInfo.commandBufferCount = 1;

        VkCommandBuffer &command_buffer = s_AllocatedCommandBuffers[wd->FrameIndex].emplace_back();
        auto err = vkAllocateCommandBuffers(g_Device, &cmdBufAllocateInfo, &command_buffer);
        inf_check_vk_result(err);
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(command_buffer, &begin_info);
        inf_check_vk_result(err);

        return command_buffer;
    }

    void Application::FlushCommandBuffer(VkCommandBuffer commandBuffer) {
        constexpr uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &commandBuffer;
        auto err = vkEndCommandBuffer(commandBuffer);
        inf_check_vk_result(err);

        // Create fence to ensure that the command buffer has finished executing
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = 0;
        VkFence fence;
        err = vkCreateFence(g_Device, &fenceCreateInfo, nullptr, &fence);
        inf_check_vk_result(err);

        err = vkQueueSubmit(g_Queue, 1, &end_info, fence);
        inf_check_vk_result(err);

        err = vkWaitForFences(g_Device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
        inf_check_vk_result(err);

        vkDestroyFence(g_Device, fence, nullptr);
    }

    void Application::SubmitResourceFree(std::function<void()> &&func) { s_ResourceFreeQueue[s_CurrentFrameIndex].emplace_back(func); }

    ImFont *Application::GetFont(const std::string &name) {
        if (!s_Fonts.contains(name))
            return nullptr;

        return s_Fonts.at(name);
    }

    void Application::SetWindowTitle(const std::string &title) {
        if (s_WindowHandle != nullptr) {
            glfwSetWindowTitle(s_WindowHandle, title.c_str());
        }
    }


} // namespace Ouroboros
