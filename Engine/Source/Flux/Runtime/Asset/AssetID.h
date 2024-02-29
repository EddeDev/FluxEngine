#pragma once

#include "Flux/Runtime/Core/Guid.h"

namespace Flux {

	using AssetID = Guid;

	namespace Utils {

		inline static AssetID GenerateAssetID()
		{
			return Guid::NewGuid();
		}

	}

}