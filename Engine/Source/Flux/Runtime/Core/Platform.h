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

		static DialogResult OpenFolderDialog(Window* window, std::string* outPath, const char* title = "Select Folder");
		static DialogResult MessageBox(MessageBoxButtons buttons, MessageBoxIcon icon, const char* text, const char* caption, Window* window = nullptr);

		static bool IsDebuggerPresent();
		static void DebugBreak();

		static bool SetConsoleTitle(const char* title);

		static bool SetThreadName(ThreadHandle handle, const char* name);
		static std::string GetThreadName(ThreadHandle handle);

		static void SetThreadPriority(ThreadHandle handle, ThreadPriority priority);
		static ThreadPriority GetThreadPriority(ThreadHandle handle);

		static ThreadHandle GetCurrentThread();
		static ThreadHandle GetThreadFromID(ThreadID threadID);
		static ThreadID GetThreadID(ThreadHandle handle);
		static ThreadID GetCurrentThreadID();

		static std::string GetErrorMessage(uint32 error = 0);
		static uint32 GetLastError();

		static std::string GetEnvironmentVariable(const char* variableName);
		static bool SetEnvironmentVariable(const char* variableName, const char* value);

		static WindowClassHandle GetWindowClass();
	};

}