#pragma once

#include "Shader.h"
#include "PixelFormat.h"

namespace Flux {

	enum class ImageUsage : uint8
	{
		None = 0,

		Texture,
		Storage,
		Attachment
	};

	struct ImageCreateInfo
	{
		const void* InitialData = nullptr;

		PixelFormat Format = PixelFormat::RGBA;
		ImageUsage Usage = ImageUsage::Texture;

		uint32 Width = 1;
		uint32 Height = 1;
		uint32 ArrayLayers = 1;
		uint32 MipLevels = 1;

		std::string DebugLabel = "Image";
	};

	class Image : public ReferenceCounted
	{
	public:
		virtual ~Image() {}

		virtual void Invalidate() = 0;
		virtual void RT_Invalidate() = 0;

		virtual void Release() = 0;
		virtual void RT_Release() = 0;

		virtual void AttachToFramebuffer(uint32 attachmentIndex) = 0;
		virtual void AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer) = 0;

		virtual void RT_AttachToFramebuffer(uint32 attachmentIndex) = 0;
		virtual void RT_AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer) = 0;

		virtual ImageCreateInfo& GetCreateInfo() = 0;
		virtual const ImageCreateInfo& GetCreateInfo() const = 0;
	};

	class Image2D : public Image
	{
	public:
		static Ref<Image2D> Create(const ImageCreateInfo& createInfo);
	};

	namespace Utils {

		inline static uint32 CalculateMipCount(uint32 width, uint32 height)
		{
			return (uint32)std::floor(std::log2(glm::min(width, height))) + 1;
		}

	}

}