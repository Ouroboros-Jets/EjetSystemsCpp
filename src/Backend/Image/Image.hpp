
#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "imgui.h"
#include "vulkan/vulkan.h"

namespace Ouroboros {
    enum class ImageFormat { None = 0, RGBA, RGBA32F };

    class Image {
    public:
        explicit Image(std::string_view path);

        Image(uint32_t width, uint32_t height, ImageFormat format, const void *data = nullptr);

        ~Image();


        void SetData(const void *data);

        [[nodiscard]] VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

        void Resize(uint32_t width, uint32_t height);

        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }

        static void *Decode(const uint8_t *data, uint64_t bin_size, uint32_t &outWidth, uint32_t &outHeight);


        /// <summary>
        /// Renders a pre-constructed image with no controls for width and height, only scale
        /// </summary>
        /// <param name="image">Shared pointer to an Infinity::Image</param>
        /// <param name="pos">X and Y position for the image</param>
        /// <param name="scale">Scale of the image 1.0f - 0.0f</param>
        static void RenderImage(const std::shared_ptr<Image> &image, ImVec2 pos, float scale);

        /// <summary>
        /// Renders a pre-constructed image with a specified width and height. The image will always fill the height requirement (if the image width > specified width, the image
        /// will be clipped evenly left and right)
        /// </summary>
        /// <param name="image">Shared pointer to an Infinity::Image</param>
        /// <param name="pos">X any Y position to render the image</param>
        /// <param name="size">Size of the image (Width, Height)</param>
        static void RenderImage(const std::shared_ptr<Image> &image, ImVec2 pos, ImVec2 size);

        static void RenderHomeImage(const std::shared_ptr<Image> &image, ImVec2 pos, ImVec2 size, bool is_hovered);

    private:
        void AllocateMemory(uint64_t size);

        void Release();

    private:
        uint32_t m_Width = 0, m_Height = 0;

        VkImage m_Image = nullptr;
        VkImageView m_ImageView = nullptr;
        VkDeviceMemory m_Memory = nullptr;
        VkSampler m_Sampler = nullptr;

        ImageFormat m_Format = ImageFormat::None;

        VkBuffer m_StagingBuffer = nullptr;
        VkDeviceMemory m_StagingBufferMemory = nullptr;

        size_t m_AlignedSize = 0;

        VkDescriptorSet m_DescriptorSet = nullptr;

        std::string m_Filepath;

        static std::map<ImGuiID, float> s_animation_progress;
    };

    class ImageStorage {
    public:
        static ImageStorage &Instance() {
            static ImageStorage instance;
            return instance;
        }

        void AddImage(const std::string &name, const std::shared_ptr<Image> &image) { m_Images[name] = image; }

        const std::shared_ptr<Image> &GetImage(const std::string &name) const { return m_Images.at(name); }

    private:
        std::map<std::string, std::shared_ptr<Image>> m_Images;
    };
} // namespace Ouroboros
