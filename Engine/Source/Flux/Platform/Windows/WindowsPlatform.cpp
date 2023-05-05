#include "FluxPCH.h"
#include "Flux/Core/Platform.h"

#ifdef FLUX_PLATFORM_WINDOWS

namespace Flux {

	void Platform::Init()
	{
		DisableProcessWindowsGhosting();

		FLUX_ASSERT(SUCCEEDED(OleInitialize(NULL)), "Failed to initialize the COM library.");
	}

	void Platform::Shutdown()
	{
		OleUninitialize();
	}

#ifndef FLUX_BUILD_SHIPPING
	bool Platform::IsDebuggerPresent()
	{
		return ::IsDebuggerPresent();
	}
#endif

	std::string Platform::GetErrorMessage(int32 error)
	{
		if (error == 0)
			return {};

		LPSTR messageBuffer = nullptr;
		DWORD size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);

		if (message[message.size() - 1] == '\n')
			message.pop_back();
		if (message[message.size() - 1] == '\r')
			message.pop_back();
		if (message[message.size() - 1] == '.')
			message.pop_back();

		return message;
	}

	uint32 Platform::GetLastError()
	{
		return static_cast<uint32>(::GetLastError());
	}

}

#endif