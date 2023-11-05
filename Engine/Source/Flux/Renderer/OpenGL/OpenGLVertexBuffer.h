#pragma once

#include "Flux/Renderer/VertexBuffer.h"

namespace Flux {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const void* data, uint64 size, VertexBufferUsage usage);
		virtual ~OpenGLVertexBuffer();
	};

}