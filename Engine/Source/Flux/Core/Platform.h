#pragma once

#include "Window.h"
#include "Thread.h"

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
		None = 0,

		Abort,
		Cancel,
		Ignore,
		No,
		Ok,
		Retry,
		Yes
	};

	using WindowClassHandle = uint32;

	class Platform
	{
	public:
		static void Init();
		static void Shutdown();

		static bool WaitMessage();
		static bool PostEmptyEvent();
		static void PumpMessages();

		static void Sleep(float seconds);

		static float GetTime();
		static uint64 GetNanoTime();

		static DialogResult MessageBox(MessageBoxButtons buttons, MessageBoxIcon icon, const std::string& text, const std::string& caption);

		static bool IsDebuggerPresent();
		static void DebugBreak();

		static bool SetConsoleTitle(const std::string& title);

		static bool SetThreadName(ThreadHandle handle, std::string_view name);
		static std::string GetThreadName(ThreadHandle handle);

		static void SetThreadPriority(ThreadHandle handle, ThreadPriority priority);
		static ThreadPriority GetThreadPriority(ThreadHandle handle);

		static ThreadHandle GetCurrentThread();
		static ThreadHandle GetThreadFromID(ThreadID threadID);
		static ThreadID GetThreadID(ThreadHandle handle);
		static ThreadID GetCurrentThreadID();

		static std::string GetErrorMessage(uint32 error = 0);
		static uint32 GetLastError();

		static WindowClassHandle GetWindowClass();
		static WindowHandle GetHelperWindowHandle();

		static int16 GetKeyCode(int32 scancode);
		static int16 GetScanCode(int16 key);
		static char* GetKeyName(int32 key);
	};

}