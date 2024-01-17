#include "FluxPCH.h"
#include "OpenGLIndexBuffer.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

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
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLIndexBufferData();
		m_Data->Storage.SetSize(size);

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, size, usage]() mutable
		{
			glCreateBuffers(1, &data->BufferID);
			glNamedBufferData(data->BufferID, size, nullptr, Utils::OpenGLBufferUsage(usage));
		});
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const void* data, uint64 size, IndexBufferDataType dataType, IndexBufferUsage usage)
		: m_DataType(dataType), m_Usage(usage)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLIndexBufferData();
		m_Data->Storage.SetSize(size);

		uint32 bufferIndex = m_Data->Storage.SetData(data, size);

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, bufferIndex, usage]() mutable
		{
			auto& buffer = data->Storage.GetBuffer(bufferIndex);
			glCreateBuffers(1, &data->BufferID);
			glNamedBufferData(data->BufferID, buffer.Size, buffer.Data, Utils::OpenGLBufferUsage(usage));
			data->Storage.SetBufferAvailable(bufferIndex);
		});
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([data = m_Data]() mutable
		{
			if (data->BufferID)
				glDeleteBuffers(1, &data->BufferID);
			delete data;
		});
	}

	void OpenGLIndexBuffer::Bind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data]()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->BufferID);
		});
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([]()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		});
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint64 size, uint64 offset)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		uint32 bufferIndex = m_Data->Storage.SetData(data, size, offset);

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, bufferIndex, size, offset]() mutable
		{
			auto& buffer = data->Storage.GetBuffer(bufferIndex);
			glNamedBufferSubData(data->BufferID, offset, size, buffer.GetData(offset));
			data->Storage.SetBufferAvailable(bufferIndex);
		});
	}

}