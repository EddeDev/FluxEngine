#pragma once

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

	namespace Utils {
		
		inline static const char* LogVerbosityToString(LogVerbosity verbosity)
		{
			switch (verbosity)
			{
			case LogVerbosity::Trace:    return "Trace";
			case LogVerbosity::Debug:    return "Debug";
			case LogVerbosity::Info:     return "Info";
			case LogVerbosity::Warning:  return "Warning";
			case LogVerbosity::Error:    return "Error";
			case LogVerbosity::Critical: return "Critical";
			}
			return "Unknown";
		}

		inline static LogVerbosity LogVerbosityFromString(std::string_view string)
		{
			if (string == "Trace")
				return LogVerbosity::Trace;
			if (string == "Debug")
				return LogVerbosity::Debug;
			if (string == "Info")
				return LogVerbosity::Info;
			if (string == "Warning")
				return LogVerbosity::Warning;
			if (string == "Error")
				return LogVerbosity::Error;
			if (string == "Critical")
				return LogVerbosity::Critical;
			return static_cast<LogVerbosity>(0);
		}
		
	}

}