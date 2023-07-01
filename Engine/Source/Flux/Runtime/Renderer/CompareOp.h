#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"

namespace Flux {

	enum class CompareOp : uint8
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

		inline static const char* CompareOpToString(CompareOp compareOp)
		{
			switch (compareOp)
			{
			case CompareOp::Never:          return "Never";
			case CompareOp::Less:           return "Less";
			case CompareOp::Equal:          return "Equal";
			case CompareOp::LessOrEqual:    return "Less or Equal";
			case CompareOp::Greater:        return "Greater";
			case CompareOp::NotEqual:       return "Not Equal";
			case CompareOp::GreaterOrEqual: return "Greater or Equal";
			case CompareOp::Always:         return "Always";
			}
			return "Unknown";
		}

	}

}