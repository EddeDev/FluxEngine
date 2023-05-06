#include "FluxPCH.h"
#include "Window.h"

#ifdef FLUX_PLATFORM_WINDOWS
	#include "Flux/Runtime/Platform/Windows/WindowsWindow.h"
#endif

namespace Flux {

	Shared<Window> Window::Create(const WindowCreateInfo& createInfo)
	{
#ifdef FLUX_PLATFORM_WINDOWS
		return CreateShared<WindowsWindow>(createInfo);
#else
	#error Unknown platform!
#endif
		return nullptr;
	}

}