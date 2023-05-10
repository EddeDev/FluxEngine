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

	enum class DialogResult : uint8
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

	using WindowMenu = void*;

	typedef std::function<void(WindowMenu, uint32)> WindowMenuCallback;

	class Platform
	{
	public:
		static void Init();
		static void Shutdown();

		static bool WaitMessage();
		static void PumpMessages();

		static void Sleep(float seconds);

		static float GetTime();
		static uint64 GetNanoTime();

		static WindowMenu CreateMenu();
		static bool SetMenu(Window* window, WindowMenu menu, WindowMenuCallback callback = {});
		static bool AddMenu(WindowMenu menu, uint32 itemID = 0, const char* name = "");
		static bool AddMenuSeparator(WindowMenu menu);
		static bool AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name = "");

		static DialogResult OpenFolderDialog(Window* window, char** outPath, const char* title = "Select Folder");
		static DialogResult MessageBox(MessageBoxButtons buttons, MessageBoxIcon icon, const char* text, const char* caption, Window* window = nullptr);

		static bool IsDebuggerPresent();
		static void DebugBreak();

		static std::string GetErrorMessage(int32 error);
		static uint32 GetLastError();

		static std::string GetEnvironmentVariable(const char* variableName);
		static bool SetEnvironmentVariable(const char* variableName, const char* value);

		static WindowClassHandle GetWindowClass();
	};

}