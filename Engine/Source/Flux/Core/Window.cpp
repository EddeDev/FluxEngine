#include "FluxPCH.h"
#include "Window.h"

#ifdef FLUX_PLATFORM_WINDOWS
	#include "Flux/Platform/Windows/WindowsWindow.h"
#endif

namespace Flux {

	Unique<Window> Window::Create(const WindowCreateInfo& createInfo)
	{
#ifdef FLUX_PLATFORM_WINDOWS
		return CreateUnique<WindowsWindow>(createInfo);
#else
	#error Unknown platform!
#endif
		return nullptr;
	}

}