#pragma once

#include "Window.h"

namespace Flux {

	enum class MessageBoxButtons : uint8
	{
		None = 0,

		AbortRetryIgnore,
		Ok,
		OkCancel,
		RetryCancel,
		YesNo,
		YesNoCancel
	};

	enum class MessageBoxIcon : uint8
	{
		None = 0,

		Asterisk,
		Error,
		Exclamation,
		Hand,
		Information,
		Question,
		Stop,
		Warning
	};

	enum class MessageBoxResult : uint8
	{
		Abort = 0,
		Cancel,
		Ignore,
		No,
		None,
		Ok,
		Retry,
		Yes
	};

	using WindowClassHandle = uint32;

	class GenericPlatform
	{
	public:
		static void Init() {}
		static void Shutdown() {}

		static void PumpMessages() {}
		static void Sleep(float seconds) {}

		static float GetTime() { return 0.0f; }
		static uint64 GetNanoTime() { return 0; }

		static MessageBoxResult MessageBox(Window* window, MessageBoxButtons buttons, MessageBoxIcon icon, const char* text, const char* caption) {}

		static bool IsDebuggerPresent()
		{
#ifdef FLUX_BUILD_SHIPPING
			return false;
#else
			return true;
#endif
		}

		static std::string GetErrorMessage(int32 error) { return "Not implemented"; }
		static uint32 GetLastError() { return 0; }

		static std::string GetEnvironmentVariable(const char* variableName) { return "Not implemented"; }
		static bool SetEnvironmentVariable(const char* variableName, const char* value) {}

		static WindowClassHandle GetWindowClass() { return 0; }
	};

}