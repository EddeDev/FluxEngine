#pragma once

#include "Flux/Platform/PlatformBase.h"

namespace Flux {

	class WindowsPlatform : public PlatformBase
	{
	public:
		static void Init();
		static void Shutdown();

		static std::string GetErrorMessage(int32 error);
		static uint32 GetLastError();
	};

}