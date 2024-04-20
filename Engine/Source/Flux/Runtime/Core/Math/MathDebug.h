#pragma once

#ifdef FLUX_BUILD_DEBUG
	#define FLUX_MATH_DEBUG_ENABLED
#endif

#ifdef FLUX_MATH_DEBUG_ENABLED
	#include "Flux/Runtime/Core/BaseTypes.h"

	#include <map>
	#include <string_view>
#endif

namespace Flux {

#ifdef FLUX_MATH_DEBUG_ENABLED
	class MathDebug
	{
	public:
		using FunctionCallMap = std::map<std::string_view, uint32>;
	public:
		inline static void Reset()
		{
			s_FunctionCalls.clear();
		}
		inline static const FunctionCallMap& GetFunctionCalls()
		{
			return s_FunctionCalls;
		}
		inline static void AddCall(std::string_view functionName)
		{
			s_FunctionCalls[functionName]++;
		}
	public:
		inline static FunctionCallMap s_FunctionCalls;
	};

	#define FLUX_MATH_PROFILE_FUNC() MathDebug::AddCall(__FUNCTION__)
#else
	#define FLUX_MATH_PROFILE_FUNC()
#endif

}