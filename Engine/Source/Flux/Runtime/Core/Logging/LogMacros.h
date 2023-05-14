#pragma once

#include "Logger.h"

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_TRACE_ENABLED
#endif

#ifdef FLUX_TRACE_ENABLED
	#define FLUX_TRACE(...) ::Flux::Logger::Log(::Flux::LogVerbosity::Trace, __VA_ARGS__)
	#define FLUX_TRACE_CATEGORY(category, ...) ::Flux::Logger::LogCategory(category, ::Flux::LogVerbosity::Trace, __VA_ARGS__)
#else
	#define FLUX_TRACE(...)
	#define FLUX_TRACE_CATEGORY(...)
#endif

#define FLUX_LOG(verbosity, ...) ::Flux::Logger::Log(verbosity, __VA_ARGS__)
#define FLUX_LOG_CATEGORY(verbosity, category, ...) ::Flux::Logger::LogCategory(category, verbosity, __VA_ARGS__)

#define FLUX_INFO(...)     ::Flux::Logger::Log(::Flux::LogVerbosity::Info,     __VA_ARGS__)
#define FLUX_WARNING(...)  ::Flux::Logger::Log(::Flux::LogVerbosity::Warning,  __VA_ARGS__)
#define FLUX_ERROR(...)    ::Flux::Logger::Log(::Flux::LogVerbosity::Error,    __VA_ARGS__)
#define FLUX_CRITICAL(...) ::Flux::Logger::Log(::Flux::LogVerbosity::Critical, __VA_ARGS__)

#define FLUX_INFO_CATEGORY(category, ...)     ::Flux::Logger::LogCategory(category, ::Flux::LogVerbosity::Info,     __VA_ARGS__)
#define FLUX_WARNING_CATEGORY(category, ...)  ::Flux::Logger::LogCategory(category, ::Flux::LogVerbosity::Warning,  __VA_ARGS__)
#define FLUX_ERROR_CATEGORY(category, ...)    ::Flux::Logger::LogCategory(category, ::Flux::LogVerbosity::Error,    __VA_ARGS__)
#define FLUX_CRITICAL_CATEGORY(category, ...) ::Flux::Logger::LogCategory(category, ::Flux::LogVerbosity::Critical, __VA_ARGS__)