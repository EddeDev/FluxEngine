#include "FluxPCH.h"
#include "Framebuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#include "OpenGL/OpenGLFramebuffer.h"

namespace Flux {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferCreateInfo& createInfo)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLFramebuffer>::Create(createInfo);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}