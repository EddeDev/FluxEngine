#pragma once

#include "BaseTypes.h"
#include "RefCounting.h"
#include "Logging/LogMacros.h"
#include "Logging/LogFormatters.h"
#include "AssertionMacros.h"
#include "Platform.h"
#include "Input.h"
#include "Buffer.h"
#include "Math/Math.h"

#define FLUX_BIND_CALLBACK(func, ...) \
[__VA_ARGS__](auto&&... args) -> decltype(auto) \
{ \
	return func(std::forward<decltype(args)>(args)...); \
}