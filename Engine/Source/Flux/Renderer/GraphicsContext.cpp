#include "FluxPCH.h"
#include "GraphicsContext.h"

#include "Flux/Core/Engine.h"

#include "OpenGL/OpenGLContext.h"

namespace Flux {

	Ref<GraphicsContext> GraphicsContext::Create(WindowHandle windowHandle)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLContext>::Create(windowHandle);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}