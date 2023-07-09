#pragma once

#include "Flux/Runtime/Renderer/RenderMaterial.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanMaterial : public RenderMaterial
	{
	public:
		VulkanMaterial(Ref<Shader> shader, const std::string& name);
		VulkanMaterial(Ref<RenderMaterial> other, const std::string& name);
		virtual ~VulkanMaterial();

		virtual void Bind(Ref<CommandBuffer> commandBuffer, Ref<GraphicsPipeline> pipeline) override;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer, Ref<GraphicsPipeline> pipeline) override;

		virtual void Set(std::string_view name, float value) override;
		virtual void Set(std::string_view name, const glm::vec2& value) override;
		virtual void Set(std::string_view name, const glm::vec3& value) override;
		virtual void Set(std::string_view name, const glm::vec4& value) override;

		virtual void Set(std::string_view name, Ref<Image2D> image) override;
	private:
		template<typename T>
		void Set(std::string_view name, const T& value)
		{
			std::lock_guard<std::mutex> lock(m_PushConstantMutex);

			auto it = m_PushConstantMembers.find(name);
			if (it == m_PushConstantMembers.end())
			{
				FLUX_WARNING_CATEGORY("Material", "Could not find push constant member: '{0}'", name);
				return;
			}

			uint32 size = sizeof(T);
			if (size > it->second.Size)
			{
				FLUX_VERIFY(false);
				size = it->second.Size;
			}

			memcpy(m_PushConstantStorage + it->second.Offset, &value, size);
		}
	private:
		void CreatePushConstantStorage();

		void CreateDescriptors();
		void RT_CreateDescriptors();
	private:
		Ref<Shader> m_Shader;
		std::string m_Name;

		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		std::unordered_map<std::string_view, ShaderPushConstantMember> m_PushConstantMembers;
		uint8* m_PushConstantStorage = nullptr;
		uint32 m_PushConstantSize = 0;
		std::mutex m_PushConstantMutex;

		// set -> type -> binding
		std::map<uint32, std::map<DescriptorType, std::map<uint32, Ref<ReferenceCounted>>>> m_Descriptors;
		std::map<uint32, VkDescriptorSet> m_DescriptorSets;
	};

}