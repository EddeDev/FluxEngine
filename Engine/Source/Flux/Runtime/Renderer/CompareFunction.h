#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"

namespace Flux {

	enum class CompareFunction : uint8
	{
		Never = 0,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always
	};

	namespace Utils {

		inline static const char* CompareFunctionToString(CompareFunction function)
		{
			switch (function)
			{
			case CompareFunction::Never: return "Never";
			case CompareFunction::Less:  return "Less";
			case CompareFunction::Equal: return "Equal";
			case CompareFunction::LessOrEqual: return "LessOrEqual";
			case CompareFunction::Greater: return "Greater";
			case CompareFunction::NotEqual: return "NotEqual";
			case CompareFunction::GreaterOrEqual: return "GreaterOrEqual";
			case CompareFunction::Always: return "Always";
			}
			return "Unknown";
		}

	}

}