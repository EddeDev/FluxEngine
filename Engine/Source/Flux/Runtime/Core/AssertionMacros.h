#pragma once

#include "Logger.h"

#ifdef FLUX_BUILD_DEBUG
	#define FLUX_ENABLE_ASSERTS
#endif

#ifdef FLUX_ENABLE_ASSERTS
	#define FLUX_ASSERT(expression, ...) if (!(expression)) ::Flux::Logger::AssertionFailed(__VA_ARGS__)
#else
	#define FLUX_ASSERT(expression, ...) (void)0
#endif

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_VERIFY(expression, ...) if (!(expression)) ::Flux::Logger::VerifyFailed(__VA_ARGS__)
#else
	#define FLUX_VERIFY(expression, ...) (void)0
#endif

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_CHECK_IS_THREAD(threadID) ::Flux::Logger::CheckIsThread(threadID, __FUNCTION__)
#else
	#define FLUX_CHECK_IS_THREAD(threadID) (void)0
#endif