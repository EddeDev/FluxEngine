#include "FluxPCH.h"
#include "OpenGLVertexBuffer.h"

#include "Flux/Core/Engine.h"

namespace Flux {

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint64 size, VertexBufferUsage usage)
	{
		FLUX_CHECK_IS_MAIN_THREAD();
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		FLUX_CHECK_IS_MAIN_THREAD();
	}

}