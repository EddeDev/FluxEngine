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

}