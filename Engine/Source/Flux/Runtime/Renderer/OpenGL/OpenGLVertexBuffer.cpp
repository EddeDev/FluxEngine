#include "FluxPCH.h"
#include "OpenGLVertexBuffer.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	namespace Utils {

		static uint32 OpenGLBufferUsage(VertexBufferUsage usage)
		{
			switch (usage)
			{
			case VertexBufferUsage::Static:  return GL_STATIC_DRAW;
			case VertexBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
			case VertexBufferUsage::Stream:  return GL_STREAM_DRAW;
			}
			FLUX_VERIFY(false, "Unknown vertex buffer usage!");
			return 0;
		}

	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint64 size, VertexBufferUsage usage)
		: m_Size(size), m_Usage(usage)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLVertexBufferData();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, size, usage]() mutable
		{
			glCreateBuffers(1, &data->BufferID);
			glNamedBufferData(data->BufferID, size, nullptr, Utils::OpenGLBufferUsage(usage));
		});
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint64 size, VertexBufferUsage usage)
		: m_Size(size), m_Usage(usage)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLVertexBufferData();

		uint32 bufferIndex = m_Data->Storage.SetData(data, size);

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, bufferIndex, usage]() mutable
		{
			Buffer buffer = data->Storage.GetBuffer(bufferIndex);
			glCreateBuffers(1, &data->BufferID);
			glNamedBufferData(data->BufferID, buffer.Size, buffer.Data, Utils::OpenGLBufferUsage(usage));
			data->Storage.SetBufferAvailable(bufferIndex);
		});
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([data = m_Data]() mutable
		{
			if (data->BufferID)
				glDeleteBuffers(1, &data->BufferID);
			delete data;
		});
	}

	void OpenGLVertexBuffer::Bind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data]()
		{
			glBindBuffer(GL_ARRAY_BUFFER, data->BufferID);
		});
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([]()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		});
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint64 size, uint64 offset)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		uint32 bufferIndex = m_Data->Storage.SetData(data, size);

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, bufferIndex, size, offset]() mutable
		{
			Buffer buffer = data->Storage.GetBuffer(bufferIndex);
			glNamedBufferSubData(data->BufferID, offset, size, buffer.GetData());
			data->Storage.SetBufferAvailable(bufferIndex);
		});
	}

}