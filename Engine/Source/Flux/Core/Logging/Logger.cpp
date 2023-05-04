#include "FluxPCH.h"
#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Flux {

	void Logger::Init()
	{
		std::array<spdlog::sink_ptr, 2> sinks = {
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/Latest.log", true),
		};

		for (auto& sink : sinks)
		{
			sink->set_level(spdlog::level::trace);
			sink->set_pattern("%^[%T] [%n]: %v%$");
		}

		s_Logger = CreateUnique<spdlog::logger>("Flux", sinks.begin(), sinks.end());
	}

	void Logger::Shutdown()
	{
		s_Logger.reset();

		spdlog::shutdown();
	}

}