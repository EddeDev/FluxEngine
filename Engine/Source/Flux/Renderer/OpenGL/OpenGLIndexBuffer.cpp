#include "FluxPCH.h"
#include "OpenGLIndexBuffer.h"

#include "Flux/Core/Engine.h"
#include "Flux/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	namespace Utils {

		static uint32 OpenGLBufferUsage(IndexBufferUsage usage)
		{
			switch (usage)
			{
			case IndexBufferUsage::Static:  return GL_STATIC_DRAW;
			case IndexBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
			case IndexBufferUsage::Stream:  return GL_STREAM_DRAW;
			}
			FLUX_VERIFY(false, "Unknown vertex buffer usage!");
			return 0;
		}

	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint64 size, IndexBufferDataType dataType, IndexBufferUsage usage)
		: m_DataType(dataType), m_Usage(usage)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage.SetSize(size);

		Ref<OpenGLIndexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, size, usage]() mutable
		{
			glCreateBuffers(1, &instance->m_BufferID);
			glNamedBufferData(instance->m_BufferID, size, nullptr, Utils::OpenGLBufferUsage(usage));
		});
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const void* data, uint64 size, IndexBufferDataType dataType, IndexBufferUsage usage)
		: m_DataType(dataType), m_Usage(usage)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage.SetSize(size);

		uint32 bufferIndex = m_Storage.SetData(data, size);

		Ref<OpenGLIndexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, bufferIndex, usage]() mutable
		{
			Buffer buffer = instance->m_Storage.GetBuffer(bufferIndex);
			glCreateBuffers(1, &instance->m_BufferID);
			glNamedBufferData(instance->m_BufferID, buffer.Size, buffer.Data, Utils::OpenGLBufferUsage(usage));
			instance->m_Storage.SetBufferAvailable(bufferIndex);
		});
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([bufferID = m_BufferID]() mutable
		{
			if (bufferID)
				glDeleteBuffers(1, &bufferID);
		});
	}

	void OpenGLIndexBuffer::Bind() const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const OpenGLIndexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_BufferID);
		});
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([]()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		});
	}

	void OpenGLIndexBuffer::Resize(uint64 size)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage.SetSize(size);

		Ref<OpenGLIndexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, size, usage = m_Usage]() mutable
		{
			glNamedBufferData(instance->m_BufferID, size, nullptr, Utils::OpenGLBufferUsage(usage));
		});
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint64 size, uint64 offset)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		uint32 bufferIndex = m_Storage.SetData(data, size, offset);

		Ref<OpenGLIndexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, bufferIndex, size, offset]() mutable
		{
			Buffer buffer = instance->m_Storage.GetBuffer(bufferIndex);
			glNamedBufferSubData(instance->m_BufferID, offset, size, buffer.GetData(offset));
			instance->m_Storage.SetBufferAvailable(bufferIndex);
		});
	}

}