#include "FluxPCH.h"
#include "IndexBuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#include "OpenGL/OpenGLIndexBuffer.h"

namespace Flux {

	Ref<IndexBuffer> IndexBuffer::Create(uint64 size, IndexBufferUsage usage)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLIndexBuffer>::Create(size, usage);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(const void* data, uint64 size, IndexBufferUsage usage)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLIndexBuffer>::Create(data, size, usage);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}