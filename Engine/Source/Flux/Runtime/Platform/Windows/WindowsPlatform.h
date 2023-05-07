#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Runtime/Core/GenericPlatform.h"

namespace Flux {

	class WindowsPlatform : public GenericPlatform
	{
	public:
		static void Init();
		static void Shutdown();

		static void PumpMessages();
		static void Sleep(float seconds);

		static float GetTime();
		static uint64 GetNanoTime();

		static MessageBoxResult MessageBox(MessageBoxButtons buttons, MessageBoxIcon icon, const char* text, const char* caption, Window* window = nullptr);

		static bool IsDebuggerPresent();
		static void DebugBreak();

		static std::string GetErrorMessage(int32 error);
		static uint32 GetLastError();

		static std::string GetEnvironmentVariable(const char* variableName);
		static bool SetEnvironmentVariable(const char* variableName, const char* value);

		static WindowClassHandle GetWindowClass();
	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

	typedef WindowsPlatform Platform;

}

#endif