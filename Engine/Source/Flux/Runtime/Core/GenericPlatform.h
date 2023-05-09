#pragma once

#include "Window.h"
#include "MessageBox.h"

namespace Flux {

	using WindowMenu = void*;

	enum WindowMenuFlags : uint32
	{
		WindowMenu_Enabled = 0,
		WindowMenu_String = 0,
		WindowMenu_Grayed = 1 << 0,
		WindowMenu_Disabled = 1 << 1,
		WindowMenu_Popup = 1 << 4,
		WindowMenu_Separator = 1 << 11
	};

	using WindowClassHandle = uint32;

	typedef std::function<void(WindowMenu, uint32)> WindowMenuCallback;

	class GenericPlatform
	{
	public:
		static void Init() {}
		static void Shutdown() {}

		static void PumpMessages() {}
		static void Sleep(float seconds) {}

		static float GetTime() { return 0.0f; }
		static uint64 GetNanoTime() { return 0; }

		static WindowMenu CreateMenu() { return nullptr; }
		static bool SetMenu(Window* window, WindowMenu menu, WindowMenuCallback callback = {}) { return false; }
		static bool AddMenu(WindowMenu menu, uint32 id = 0, const char* name = "") { return false; }
		static bool AddMenuSeparator(WindowMenu menu) { return false; }
		static bool AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name = "") { return false; }

		static MessageBoxResult MessageBox(MessageBoxButtons buttons, MessageBoxIcon icon, const char* text, const char* caption, Window* window = nullptr) { return MessageBoxResult::Cancel; }

		static bool IsDebuggerPresent()
		{
#ifdef FLUX_BUILD_SHIPPING
			return false;
#else
			return true;
#endif
		}

		static void DebugBreak() {}

		static std::string GetErrorMessage(int32 error) { return "Not implemented"; }
		static uint32 GetLastError() { return 0; }

		static std::string GetEnvironmentVariable(const char* variableName) { return "Not implemented"; }
		static bool SetEnvironmentVariable(const char* variableName, const char* value) {}

		static WindowClassHandle GetWindowClass() { return 0; }
	};

}