#pragma once

namespace Flux {

#ifdef FLUX_PLATFORM_WINDOWS
	#define FLUX_DEBUG_BREAK_IMPL() (__nop(), __debugbreak())
#endif

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_DEBUG_BREAK() if (Platform::IsDebuggerPresent()) FLUX_DEBUG_BREAK_IMPL()
#else
	#define FLUX_DEBUG_BREAK()
#endif

	class Platform
	{
	public:
		static void Init();
		static void Shutdown();

#ifndef FLUX_BUILD_SHIPPING
		static bool IsDebuggerPresent();
#endif

		static std::string GetErrorMessage(int32 error);

		static uint32 GetLastError();
	};

}