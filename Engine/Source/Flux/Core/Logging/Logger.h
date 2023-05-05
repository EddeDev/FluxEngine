#pragma once

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
	private:
		inline static Unique<spdlog::logger> s_Logger;
	};

}

