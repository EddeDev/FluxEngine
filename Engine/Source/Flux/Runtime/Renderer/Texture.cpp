#include "FluxPCH.h"
#include "Texture.h"

#include "Flux/Runtime/Core/Engine.h"

#include "OpenGL/OpenGLTexture.h"

namespace Flux {

	Ref<Texture2D> Texture2D::Create(uint32 width, uint32 height, TextureFormat format)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLTexture2D>::Create(width, height, format);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}