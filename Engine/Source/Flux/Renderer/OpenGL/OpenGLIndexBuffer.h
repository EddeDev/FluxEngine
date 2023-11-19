#pragma once

#include "Flux/Renderer/IndexBuffer.h"

namespace Flux {

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint64 size, IndexBufferDataType dataType, IndexBufferUsage usage);
		OpenGLIndexBuffer(const void* data, uint64 size, IndexBufferDataType dataType, IndexBufferUsage usage);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Resize(uint64 size) override;
		virtual void SetData(const void* data, uint64 size, uint64 offset = 0) override;

		virtual uint64 GetSize() const override { return m_Storage.GetSize(); }

		virtual IndexBufferDataType GetDataType() const override { return m_DataType; }
		virtual IndexBufferUsage GetUsage() const override { return m_Usage; }
	private:
		IndexBufferDataType m_DataType;
		IndexBufferUsage m_Usage;

		RenderThreadStorage m_Storage;

		uint32 m_BufferID = 0;
	};

}