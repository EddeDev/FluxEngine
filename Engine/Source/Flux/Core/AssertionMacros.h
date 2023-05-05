#pragma once

#include "Logging/Logger.h"

#if defined(FLUX_PLATFORM_WINDOWS)
	#define FLUX_PLATFORM_BREAK() __debugbreak()
#else
	#define FLUX_PLATFORM_BREAK()
#endif

#ifdef FLUX_BUILD_DEBUG
	#define FLUX_ENABLE_ASSERTS
#endif

#ifdef FLUX_ENABLE_ASSERTS
	#define FLUX_ASSERT(expression, ...) \
		if (!(expression)) \
		{ \
			FLUX_ERROR("Assertion failed", __VA_ARGS__); \
			FLUX_PLATFORM_BREAK(); \
		}
#else
#define FLUX_ASSERT(expression, ...)
#endif

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_VERIFY(expression, ...) \
		if (!(expression)) \
		{ \
			FLUX_ERROR("Verify failed", __VA_ARGS__); \
			FLUX_PLATFORM_BREAK(); \
		}
#else
	#define FLUX_VERIFY(expression, ...)
#endif