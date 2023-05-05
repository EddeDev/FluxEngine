#pragma once

#include "Logger.h"

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_TRACE_ENABLED
#endif

#ifdef LQ_TRACE_ENABLED
	#define FLUX_TRACE(...) ::Flux::Logger::Log(::Flux::LogVerbosity::Trace, __VA_ARGS__)
#else
	#define FLUX_TRACE(...)
#endif

#define FLUX_INFO(...)     ::Flux::Logger::Log(::Flux::LogVerbosity::Info,     __VA_ARGS__)
#define FLUX_WARNING(...)  ::Flux::Logger::Log(::Flux::LogVerbosity::Warning,  __VA_ARGS__)
#define FLUX_ERROR(...)    ::Flux::Logger::Log(::Flux::LogVerbosity::Error,    __VA_ARGS__)
#define FLUX_CRITICAL(...) ::Flux::Logger::Log(::Flux::LogVerbosity::Critical, __VA_ARGS__)

// #define FLUX_INFO_CATEGORY(category, ...) ::Flux::Logger::Log(category, ::Flux::LogVerbosity::Info,     __VA_ARGS__)