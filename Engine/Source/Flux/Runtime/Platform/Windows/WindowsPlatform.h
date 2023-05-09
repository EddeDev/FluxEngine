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

		static WindowMenu CreateMenu();
		static bool SetMenu(Window* window, WindowMenu menu, WindowMenuCallback callback = {});
		static bool AddMenu(WindowMenu menu, uint32 id = 0, const char* name = "");
		static bool AddMenuSeparator(WindowMenu menu);
		static bool AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name = "");

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
	private:
		struct MenuData
		{
			WindowMenu Menu = nullptr;
			WindowMenuCallback Callback;
		};

		inline static std::map<Window*, MenuData> s_Menus;
	};

	typedef WindowsPlatform Platform;

}

#endif