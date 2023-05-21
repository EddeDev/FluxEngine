#pragma once

#include "CommandBuffer.h"

namespace Flux {

	class IndexBuffer : public ReferenceCounted
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const = 0;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const = 0;

		static Ref<IndexBuffer> Create(const void* data, uint32 size);
	};

}