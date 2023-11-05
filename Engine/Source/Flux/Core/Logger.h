#pragma once

#include "Platform.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace Flux {

	enum class LogVerbosity : uint8
	{
		Trace = 0,
		Debug,
		Info,
		Warning,
		Error,
		Critical
	};

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

		template<typename... TArgs>
		static void LogCategory(std::string_view category, LogVerbosity verbosity, fmt::format_string<TArgs...> fmt, TArgs&&... args)
		{
			auto level = static_cast<spdlog::level::level_enum>(verbosity);
			s_Logger->log(level, "[{0}] {1}", category, fmt::format(fmt, std::forward<TArgs>(args)...));
		}

		template<typename... TArgs>
		static void AssertionFailed(fmt::format_string<TArgs...> fmt, TArgs&&... args)
		{
			auto expression = fmt::format(fmt, std::forward<TArgs>(args)...);
			s_Logger->error(fmt::format("Assertion failed: {0}", expression));
			AssertMessageBox("Assertion failed!", expression);
		}

		static void AssertionFailed()
		{
			std::string_view message = "Assertion failed!";
			s_Logger->error(message);
			AssertMessageBox(message);
		}

		template<typename... TArgs>
		static void VerifyFailed(fmt::format_string<TArgs...> fmt, TArgs&&... args)
		{
			auto expression = fmt::format(fmt, std::forward<TArgs>(args)...);
			s_Logger->error(fmt::format("Verify failed: {0}", expression));
			AssertMessageBox("Verify failed!", expression);
		}

		static void VerifyFailed()
		{
			std::string_view message = "Verify failed!";
			s_Logger->error(message);
			AssertMessageBox(message);
		}

		template<typename TThreadID>
		static void CheckIsThread(TThreadID threadID, std::string_view functionName = "")
		{
			static_assert(std::is_same<TThreadID, ThreadID>::value);

			if (Platform::GetCurrentThreadID() != threadID)
			{
				auto threadName = Platform::GetThreadName(Platform::GetThreadFromID(threadID));
				VerifyFailed("'{0}' must only be called from '{1}'.", functionName, threadName);
			}
		}
	private:
		static void AssertMessageBox(std::string_view message, std::string_view expression = "");
	private:
		inline static Unique<spdlog::logger> s_Logger;
	};

}