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
		inline T Log2(T value)
		{
			return log2(value);
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
		inline constexpr T Pow(T f, T p)
		{
			return pow(f, p);
		}

		template<typename T>
		inline constexpr T Exp(T power)
		{
			return exp(power);
		}

		template<typename T>
		inline static bool EpsilonEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon())
		{
			return Abs(a - b) < epsilon;
		}

		template<typename T>
		inline static bool EpsilonNotEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon())
		{
			return Abs(a - b) >= epsilon;
		}

		template<typename T>
		inline constexpr T Min(T a, T b)
		{
			return a < b ? a : b;
		}

		template<typename T>
		inline constexpr T Max(T a, T b)
		{
			return a > b ? a : b;
		}

		template<typename T>
		inline constexpr T Clamp(T value, T min, T max)
		{
			if (value < min)
				value = min;
			else if (value > max)
				value = max;
			return value;
		}

		template<typename T>
		inline constexpr T Lerp(T a, T b, T t)
		{
			return a + (b - a) * Clamp(t, static_cast<T>(0), static_cast<T>(1));
		}

		template<typename T>
		inline constexpr T LerpUnclamped(T a, T b, T t)
		{
			return a + (b - a) * t;
		}

		template<typename T>
		inline constexpr T GammaToLinearSpace(T value)
		{
			return Pow(value, static_cast<T>(2.2));
		}

		template<typename T>
		inline constexpr T LinearToGammaSpace(T value)
		{
			return Pow(value, static_cast<T>(1.0 / 2.2));
		}

	}

}