#pragma once

#include "CommandBuffer.h"

namespace Flux {

	class VertexBuffer : public ReferenceCounted
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const = 0;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const = 0;

		static Ref<VertexBuffer> Create(const void* data, uint32 size);
	};

}