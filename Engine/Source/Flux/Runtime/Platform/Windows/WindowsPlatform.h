#pragma once

#include "Flux/Runtime/Core/GenericPlatform.h"

namespace Flux {

	class WindowsPlatform : public GenericPlatform
	{
	public:
		static void Init();
		static void Tick();
		static void Shutdown();

		static void Sleep(float seconds);

		static float GetTime();
		static uint64 GetNanoTime();

		static MessageBoxResult MessageBox(Window* window, MessageBoxButtons buttons, MessageBoxIcon icon, const char* text, const char* caption);

		static bool IsDebuggerPresent();
		static void DebugBreak();

		static std::string GetErrorMessage(int32 error);
		static uint32 GetLastError();

		static std::string GetEnvironmentVariable(const char* variableName);
		static bool SetEnvironmentVariable(const char* variableName, const char* value);
	};

	typedef WindowsPlatform Platform;

}