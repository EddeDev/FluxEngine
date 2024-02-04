#pragma once

#include "Flux/Runtime/Renderer/IndexBuffer.h"

namespace Flux {

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint64 size, IndexBufferUsage usage);
		OpenGLIndexBuffer(const void* data, uint64 size, IndexBufferUsage usage);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint64 size, uint64 offset = 0) override;

		virtual uint64 GetSize() const override { return m_Data->Storage.GetSize(); }

		virtual IndexBufferUsage GetUsage() const override { return m_Usage; }
	private:
		IndexBufferUsage m_Usage;

		struct OpenGLIndexBufferData
		{
			RenderThreadStorage Storage;
			uint32 BufferID;
		};

		OpenGLIndexBufferData* m_Data = nullptr;
	};

}