#pragma once

#include "Flux/Renderer/IndexBuffer.h"

namespace Flux {

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const void* data, uint64 size);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		RenderThreadStorage m_Storage;
		uint32 m_BufferID = 0;
	};

}