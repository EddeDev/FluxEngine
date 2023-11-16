#include "FluxPCH.h"
#include "IndexBuffer.h"

#include "Flux/Core/Engine.h"

#include "OpenGL/OpenGLIndexBuffer.h"

namespace Flux {

	Ref<IndexBuffer> IndexBuffer::Create(const void* data, uint64 size)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLIndexBuffer>::Create(data, size);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}