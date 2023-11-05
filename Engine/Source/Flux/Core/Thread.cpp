#include "FluxPCH.h"
#include "Thread.h"

#ifdef FLUX_PLATFORM_WINDOWS
	#include "Flux/Platform/Windows/WindowsThread.h"
#endif

namespace Flux {

	Unique<Thread> Thread::Create(const ThreadCreateInfo& createInfo)
	{
#ifdef FLUX_PLATFORM_WINDOWS
		return CreateUnique<WindowsThread>(createInfo);
#else
	#error Unknown platform!
#endif
		return nullptr;
	}

}