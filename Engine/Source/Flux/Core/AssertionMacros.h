#pragma once

#include "Logging/Logger.h"

#define FLUX_LEGACY_ASSERTS 0

#if FLUX_LEGACY_ASSERTS
	#if defined(FLUX_PLATFORM_WINDOWS)
		#define FLUX_PLATFORM_BREAK() __debugbreak()
	#else
		#define FLUX_PLATFORM_BREAK()
	#endif
#endif

#ifdef FLUX_BUILD_DEBUG
	#define FLUX_ENABLE_ASSERTS
#endif

#ifdef FLUX_ENABLE_ASSERTS
	#define FLUX_ASSERT(expression, ...) if (!(expression)) ::Flux::Logger::Assert(__VA_ARGS__)
#else
	#define FLUX_ASSERT(expression, ...)
#endif

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_VERIFY(expression, ...) if (!(expression)) ::Flux::Logger::Verify(__VA_ARGS__)
#else
	#define FLUX_VERIFY(expression, ...)
#endif