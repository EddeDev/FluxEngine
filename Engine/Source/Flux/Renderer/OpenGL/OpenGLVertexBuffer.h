#pragma once

#include "Flux/Renderer/VertexBuffer.h"

namespace Flux {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint64 size, VertexBufferUsage usage);
		OpenGLVertexBuffer(const void* data, uint64 size, VertexBufferUsage usage);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint64 size, uint64 offset = 0) override;
	private:
		RenderThreadStorage m_Storage;
		uint32 m_BufferID = 0;
	};

}