#include "FluxPCH.h"
#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Flux {

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_HAS_CONSOLE
#endif

	void Logger::Init()
	{
		std::vector<spdlog::sink_ptr> sinks = {
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
#ifdef FLUX_HAS_CONSOLE
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("Logs/Latest.log", true),
#endif
		};

		sinks[0]->set_pattern("[%T] [%n]: %v");
#ifdef FLUX_HAS_CONSOLE
		sinks[1]->set_pattern("%^[%T] [%n]: %v%$");
#endif

		s_Logger = CreateUnique<spdlog::logger>("Flux", sinks.begin(), sinks.end());
		s_Logger->set_level(spdlog::level::trace);
	}

	void Logger::Shutdown()
	{
		s_Logger.reset();

		spdlog::shutdown();
	}

}