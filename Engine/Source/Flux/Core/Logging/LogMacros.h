#pragma once

#include "Logger.h"

#define FLUX_TRACE(...)    ::Flux::Logger::Log(::Flux::LogVerbosity::Trace,    __VA_ARGS__)
#define FLUX_DEBUG(...)    ::Flux::Logger::Log(::Flux::LogVerbosity::Debug,    __VA_ARGS__)
#define FLUX_INFO(...)     ::Flux::Logger::Log(::Flux::LogVerbosity::Info,     __VA_ARGS__)
#define FLUX_WARNING(...)  ::Flux::Logger::Log(::Flux::LogVerbosity::Warning,  __VA_ARGS__)
#define FLUX_ERROR(...)    ::Flux::Logger::Log(::Flux::LogVerbosity::Error,    __VA_ARGS__)
#define FLUX_CRITICAL(...) ::Flux::Logger::Log(::Flux::LogVerbosity::Critical, __VA_ARGS__)