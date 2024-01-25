#pragma once

namespace Flux {

	namespace Math {

		inline constexpr float PI = 3.1415926535897932f;
		inline constexpr float DegToRad = PI / 180.0f;
		inline constexpr float RadToDeg = 1.0f / DegToRad;

		template<typename T>
		inline T Sin(T value)
		{
			return sin(value);
		}

		template<typename T>
		inline T Cos(T value)
		{
			return cos(value);
		}

		template<typename T>
		inline T Tan(T value)
		{
			return tan(value);
		}

		template<typename T>
		inline T Asin(T value)
		{
			return asin(value);
		}

		template<typename T>
		inline T Acos(T value)
		{
			return acos(value);
		}

		template<typename T>
		inline T Atan(T x)
		{
			return atan(x);
		}

		template<typename T>
		inline T Atan2(T y, T x)
		{
			return atan2(y, x);
		}

		template<typename T>
		inline T Sqrt(T value)
		{
			return sqrt(value);
		}

		template<typename T>
		inline T Abs(T value)
		{
			return abs(value);
		}

		template<typename T>
		inline T Log(T value)
		{
			return log(value);
		}

		template<typename T>
		inline T Log10(T value)
		{
			return log10(value);
		}

		template<typename T>
		inline T Ceil(T value)
		{
			return ceil(value);
		}

		template<typename T>
		inline T Floor(T value)
		{
			return floor(value);
		}

		template<typename T>
		inline T Round(T value)
		{
			return round(value);
		}

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

		template<typename T>
		inline T Clamp(T value, T min, T max)
		{
			if (value < min)
				value = min;
			else if (value > max)
				value = max;
			return value;
		}

	}

}