#include "FluxPCH.h"
#include "OpenGLIndexBuffer.h"

#include "Flux/Core/Engine.h"
#include "Flux/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	OpenGLIndexBuffer::OpenGLIndexBuffer(const void* data, uint64 size)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage.Allocate(size);

		uint32 bufferIndex = m_Storage.SetData(data, size);

		Ref<OpenGLIndexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, bufferIndex]() mutable
		{
			Buffer buffer = instance->m_Storage.GetBuffer(bufferIndex);
			glCreateBuffers(1, &instance->m_BufferID);
			glNamedBufferData(instance->m_BufferID, buffer.Size, buffer.Data, GL_STATIC_DRAW);
			instance->m_Storage.SetBufferAvailable(bufferIndex);
		});
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([bufferID = m_BufferID]() mutable
		{
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

}