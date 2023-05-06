#pragma once

namespace Flux {

#ifdef FLUX_PLATFORM_WINDOWS
	class WindowsPlatform;
	typedef WindowsPlatform Platform;
#endif

}