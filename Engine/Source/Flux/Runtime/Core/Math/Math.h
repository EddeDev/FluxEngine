#pragma once

namespace Flux {

	namespace Math {

		template<typename T>
		inline T Min(T a, T b)
		{
			return a < b ? a : b;
		}

		template<typename T>
		inline T Max(T a, T b)
		{
			return a > b ? a : b;
		}

	}

}