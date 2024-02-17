#include "FluxPCH.h"
#include "Texture.h"

#include "Flux/Runtime/Core/Engine.h"

#include "OpenGL/OpenGLTexture.h"

namespace Flux {

	Ref<Texture> Texture::Create(const TextureProperties& properties, const void* data)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLTexture>::Create(properties, data);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}