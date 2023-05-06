#pragma once

#include "PlatformTypes.h"

namespace Flux {

	class PlatformBase
	{
	public:
		static void Init() {}
		static void Shutdown() {}

		static std::string GetErrorMessage(int32 error) { return "Not implemented"; }
		static uint32 GetLastError() { return 0; }
	};

}