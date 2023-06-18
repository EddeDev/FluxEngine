#pragma once

#include "Flux/Runtime/Core/AssertionMacros.h"
#include "Flux/Runtime/Renderer/CompareOp.h"

#include "Vulkan.h"

namespace Flux {

	namespace Utils {

		static VkCompareOp VulkanCompareOp(CompareOp compareOp)
		{
			switch (compareOp)
			{
			case CompareOp::Never:          return VK_COMPARE_OP_NEVER;
			case CompareOp::Less:           return VK_COMPARE_OP_LESS;
			case CompareOp::Equal:          return VK_COMPARE_OP_EQUAL;
			case CompareOp::LessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
			case CompareOp::Greater:        return VK_COMPARE_OP_GREATER;
			case CompareOp::NotEqual:       return VK_COMPARE_OP_NOT_EQUAL;
			case CompareOp::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case CompareOp::Always:         return VK_COMPARE_OP_ALWAYS;
			}
			FLUX_VERIFY(false, "Unknown comparison function");
			return static_cast<VkCompareOp>(0);
		}


	}

}