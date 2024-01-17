#pragma once

#include "BaseTypes.h"

namespace Flux {

	enum class BuildConfiguration : uint8
	{
		None = 0,

		Debug,
		Release,
		Shipping
	};

	namespace Utils {

		inline static const char* BuildConfigurationToString(BuildConfiguration configuration)
		{
			switch (configuration)
			{
			case BuildConfiguration::Debug: return "Debug";
			case BuildConfiguration::Release: return "Release";
			case BuildConfiguration::Shipping: return "Shipping";
			}
			return "Unknown";
		}

	}

}