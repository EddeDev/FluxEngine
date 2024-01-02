#include "FluxPCH.h"
#include "VertexBuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#include "OpenGL/OpenGLVertexBuffer.h"

namespace Flux {

	Ref<VertexBuffer> VertexBuffer::Create(uint64 size, VertexBufferUsage usage)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLVertexBuffer>::Create(size, usage);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint64 size, VertexBufferUsage usage)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLVertexBuffer>::Create(data, size, usage);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}