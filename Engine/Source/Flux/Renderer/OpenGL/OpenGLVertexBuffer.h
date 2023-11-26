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

		virtual uint64 GetSize() const override { return m_Data->Storage.GetSize(); }

		virtual VertexBufferUsage GetUsage() const override { return m_Usage; }
	private:
		VertexBufferUsage m_Usage;

		struct OpenGLVertexBufferData
		{
			RenderThreadStorage Storage;
			uint32 BufferID = 0;
		};

		OpenGLVertexBufferData* m_Data = nullptr;
	};

}