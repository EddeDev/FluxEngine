#pragma once

#include "BaseTypes.h"
#include "RefCounting.h"
#include "LogMacros.h"
#include "AssertionMacros.h"
#include "Platform.h"

#define FLUX_BIND_CALLBACK(func, ...) \
[__VA_ARGS__](auto&&... args) -> decltype(auto) \
{ \
	return func(std::forward<decltype(args)>(args)...); \
}