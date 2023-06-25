#pragma once

#include "CommandBuffer.h"

namespace Flux {

	struct Uniform
	{
		std::string Name;
		uint32 Size = 0;
		uint32 Offset = 0;
	};

	struct UniformBufferCreateInfo
	{
		std::unordered_map<std::string, Uniform> Uniforms;
		uint32 Count = 1;
		uint32 Size = 0;
		uint32 Binding = 0;
	};

	class UniformBuffer : public ReferenceCounted
	{
	public:
		virtual ~UniformBuffer() {}

		virtual void SetData(const void* data, uint32 size, uint32 offset = 0) = 0;
		virtual void RT_SetData(const void* data, uint32 size, uint32 offset = 0) = 0;

		virtual const std::unordered_map<std::string, Uniform>& GetUniforms() const = 0;

		virtual uint32 GetSize() const = 0;
		virtual uint32 GetBinding() const = 0;

		template<typename T>
		void Set(const std::string& name, const T& value)
		{
			auto& uniforms = GetUniforms();

			auto it = uniforms.find(name);
			if (it == uniforms.end())
			{
				FLUX_VERIFY(false, "Could not find uniform: {0}", name);
				return;
			}

			SetData(&value, it->second.Size, it->second.Offset);
		}

		static Ref<UniformBuffer> Create(const UniformBufferCreateInfo& createInfo);
	};

}