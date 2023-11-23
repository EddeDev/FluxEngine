#include "FluxPCH.h"
#include "OpenGLVertexBuffer.h"

#include "Flux/Core/Engine.h"
#include "Flux/Renderer/Renderer.h"

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
		: m_Usage(usage)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage.SetSize(size);

		Ref<OpenGLVertexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, size, usage]() mutable
		{
			glCreateBuffers(1, &instance->m_BufferID);
			glNamedBufferData(instance->m_BufferID, size, nullptr, Utils::OpenGLBufferUsage(usage));
		});
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint64 size, VertexBufferUsage usage)
		: m_Usage(usage)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage.SetSize(size);

		uint32 bufferIndex = m_Storage.SetData(data, size);

		Ref<OpenGLVertexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, bufferIndex, usage]() mutable
		{
			Buffer buffer = instance->m_Storage.GetBuffer(bufferIndex);
			glCreateBuffers(1, &instance->m_BufferID);
			glNamedBufferData(instance->m_BufferID, buffer.Size, buffer.Data, Utils::OpenGLBufferUsage(usage));
			instance->m_Storage.SetBufferAvailable(bufferIndex);
		});
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([bufferID = m_BufferID]() mutable
		{
			if (bufferID)
				glDeleteBuffers(1, &bufferID);
		});
	}

	void OpenGLVertexBuffer::Bind() const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const OpenGLVertexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]()
		{
			glBindBuffer(GL_ARRAY_BUFFER, instance->m_BufferID);
		});
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([]()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		});
	}

	void OpenGLVertexBuffer::Resize(uint64 size)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage.SetSize(size);

		Ref<OpenGLVertexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, size, usage = m_Usage]() mutable
		{
			glNamedBufferData(instance->m_BufferID, size, nullptr, Utils::OpenGLBufferUsage(usage));
		});
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint64 size, uint64 offset)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		uint32 bufferIndex = m_Storage.SetData(data, size, offset);

		Ref<OpenGLVertexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, bufferIndex, size, offset]() mutable
		{
			Buffer buffer = instance->m_Storage.GetBuffer(bufferIndex);
			glNamedBufferSubData(instance->m_BufferID, offset, size, buffer.GetData(offset));
			instance->m_Storage.SetBufferAvailable(bufferIndex);
		});
	}

}