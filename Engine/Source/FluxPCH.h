#pragma once

#ifdef FLUX_PLATFORM_WINDOWS
	#define OEMRESOURCE
	#define NOMINMAX
	#include <Windows.h>

	#ifdef UNICODE
		#if defined _M_IX86
			#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
		#elif defined _M_IA64
			#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
		#elif defined _M_X64
			#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
		#else
			#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
		#endif
	#endif
#endif

#include <iostream>
#include <memory>
#include <array>
#include <vector>
#include <initializer_list>
#include <queue>
#include <unordered_map>
#include <map>
#include <functional>
#include <fstream>
#include <filesystem>
#include <mutex>

#include "Flux/Runtime/Core/Core.h"