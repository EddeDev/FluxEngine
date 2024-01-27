#include "FluxPCH.h"
#include "GraphicsContext.h"

#include "Flux/Runtime/Core/Engine.h"

#include "OpenGL/OpenGLContext.h"
#include "DX11/DX11Context.h"

namespace Flux {

	Ref<GraphicsContext> GraphicsContext::Create(WindowHandle windowHandle)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLContext>::Create(windowHandle);
		case GraphicsAPI::DX11: return Ref<DX11Context>::Create(windowHandle);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}