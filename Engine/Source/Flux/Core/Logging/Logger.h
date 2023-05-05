#pragma once

#include "LogVerbosity.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace Flux {

	class Logger
	{
	public:
		static void Init();
		static void Shutdown();
		
		template<typename... TArgs>
		static void Log(LogVerbosity verbosity, fmt::format_string<TArgs...> fmt, TArgs&&... args)
		{
			auto level = static_cast<spdlog::level::level_enum>(verbosity);
			s_Logger->log(level, fmt, std::forward<TArgs>(args)...);
		}
	public:
		template<typename... TArgs>
		static void Assert(fmt::format_string<TArgs...> fmt, TArgs&&... args)
		{
			std::string message = fmt::format(fmt, std::forward<TArgs>(args)...);
			s_Logger->error(fmt::format("Assertion failed: {0}", message));
			AssertMessageBox(message);
		}
		
		static void Assert()
		{
			std::string message = "Assertion failed.";
			s_Logger->error(message);
			AssertMessageBox(message);
		}

		template<typename... TArgs>
		static void Verify(fmt::format_string<TArgs...> fmt, TArgs&&... args)
		{
			std::string message = fmt::format(fmt, std::forward<TArgs>(args)...);
			s_Logger->error(fmt::format("Verify failed: {0}", message));
			AssertMessageBox(message);
		}

		static void Verify()
		{
			std::string message = "Verify failed.";
			s_Logger->error(message);
			AssertMessageBox(message);
		}
	private:
		static void AssertMessageBox(std::string_view message)
		{
			// TODO: move to separate file
#ifdef FLUX_PLATFORM_WINDOWS
			std::string text;
			text += message;

			if (IsDebuggerPresent())
			{
				text += "\n\n";
				text += "Do you want to debug the crash?";
			}

			uint32 messageType = MB_SYSTEMMODAL | MB_ICONERROR;
			if (IsDebuggerPresent())
				messageType |= MB_YESNO;

			int32 result = MessageBoxA(NULL, text.c_str(), "Flux Assert", messageType);
			if (IsDebuggerPresent())
			{
				if (result == IDYES)
					DebugBreak();
			}
#endif
		}
	private:
		inline static Unique<spdlog::logger> s_Logger;
	};

}