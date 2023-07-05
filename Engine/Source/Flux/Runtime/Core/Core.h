#pragma once

#include "BaseTypes.h"
#include "RefCounting.h"
#include "Logging/LogMacros.h"
#include "Logging/LogFormatters.h"
#include "Debugging/Timer.h"
#include "AssertionMacros.h"
#include "Platform.h"
#include "Input.h"

#define FLUX_BIND_CALLBACK(func, ...) \
[__VA_ARGS__](auto&&... args) -> decltype(auto) \
{ \
	return func(std::forward<decltype(args)>(args)...); \
}