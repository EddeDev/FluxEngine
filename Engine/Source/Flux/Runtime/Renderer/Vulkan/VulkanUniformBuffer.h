#pragma once

#include "Flux/Runtime/Renderer/UniformBuffer.h"

#include "Vulkan.h"
#include "VulkanResourceAllocator.h"

namespace Flux {

	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(const UniformBufferCreateInfo& createInfo);
		virtual ~VulkanUniformBuffer();

		virtual void SetData(const void* data, uint32 size, uint32 offset = 0) override;
		virtual void RT_SetData(const void* data, uint32 size, uint32 offset = 0) override;

		virtual const std::unordered_map<std::string, Uniform>& GetUniforms() const override { return m_CreateInfo.Uniforms; }

		virtual uint32 GetSize() const override { return m_CreateInfo.Size; }
		virtual uint32 GetBinding() const override { return m_CreateInfo.Binding; }

		VkBuffer GetBuffer() const { return m_Buffer; }
	private:
		UniformBufferCreateInfo m_CreateInfo;

		enum class BufferState : uint8
		{
			Available = 0,
			InUse = 1,
		};

		struct Buffer
		{
			uint8* Storage = nullptr;
			BufferState State = BufferState::Available;
		};

		std::vector<Buffer> m_BufferPool;
		std::mutex m_BufferPoolMutex;

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		ResourceAllocation m_Allocation = nullptr;
	};

}