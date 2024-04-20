#pragma once

#include "MathUtils.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Vector2
	{
		union
		{
			struct
			{
				float X, Y;
			};

			struct
			{
				float R, G;
			};
		};

		Vector2()
		{
		}

		explicit Vector2(float scalar)
			: X(scalar), Y(scalar)
		{
		}

		Vector2(float x, float y)
			: X(x), Y(y)
		{
		}

		Vector2(const Vector2& other)
			: X(other.X), Y(other.Y)
		{
		}

		struct Vector2Wrapper
		{
			float& X;
			float& Y;

			Vector2 operator-() const
			{
				return { -X, -Y };
			}

			Vector2 operator+(const Vector2& v) const
			{
				Vector2 result;
				result.X = X + v.X;
				result.Y = Y + v.Y;
				return result;
			}

			Vector2Wrapper& operator+=(const Vector2& v)
			{
				X += v.X;
				Y += v.Y;
				return *this;
			}

			Vector2 operator-(const Vector2& v) const
			{
				Vector2 result;
				result.X = X - v.X;
				result.Y = Y - v.Y;
				return result;
			}

			Vector2Wrapper& operator-=(const Vector2& v)
			{
				X -= v.X;
				Y -= v.Y;
				return *this;
			}

			Vector2 operator*(const Vector2& v) const
			{
				Vector2 result;
				result.X = X * v.X;
				result.Y = Y * v.Y;
				return result;
			}

			Vector2 operator*(float scalar) const
			{
				Vector2 result;
				result.X = X * scalar;
				result.Y = Y * scalar;
				return result;
			}

			Vector2Wrapper& operator*=(float scalar)
			{
				X *= scalar;
				Y *= scalar;
				return *this;
			}

			Vector2 operator/(const Vector2& v) const
			{
				Vector2 result;
				result.X = X / v.X;
				result.Y = Y / v.Y;
				return result;
			}

			Vector2 operator/(float scalar) const
			{
				Vector2 result;
				result.X = X / scalar;
				result.Y = Y / scalar;
				return result;
			}

			Vector2Wrapper& operator/=(float scalar)
			{
				X /= scalar;
				Y /= scalar;
				return *this;
			}

			operator Vector2() const
			{
				return { X, Y };
			}
		};

		struct Vector2WrapperConst
		{
			const float& X;
			const float& Y;

			Vector2 operator-() const
			{
				return { -X, -Y };
			}

			Vector2 operator+(const Vector2& v) const
			{
				Vector2 result;
				result.X = X + v.X;
				result.Y = Y + v.Y;
				return result;
			}

			Vector2 operator-(const Vector2& v) const
			{
				Vector2 result;
				result.X = X - v.X;
				result.Y = Y - v.Y;
				return result;
			}

			Vector2 operator*(const Vector2& v) const
			{
				Vector2 result;
				result.X = X * v.X;
				result.Y = Y * v.Y;
				return result;
			}

			Vector2 operator*(float scalar) const
			{
				Vector2 result;
				result.X = X * scalar;
				result.Y = Y * scalar;
				return result;
			}

			Vector2 operator/(const Vector2& v) const
			{
				Vector2 result;
				result.X = X / v.X;
				result.Y = Y / v.Y;
				return result;
			}

			Vector2 operator/(float scalar) const
			{
				Vector2 result;
				result.X = X / scalar;
				result.Y = Y / scalar;
				return result;
			}
		};

#define SWIZZLE_VECTOR2_FUNC(x, y, ...) \
Vector2Wrapper __VA_ARGS__ ##x##y() { return { x, y }; } \
Vector2WrapperConst __VA_ARGS__ ##x##y() const { return { x, y }; }

		SWIZZLE_VECTOR2_FUNC(X, X)
		SWIZZLE_VECTOR2_FUNC(X, Y)
		SWIZZLE_VECTOR2_FUNC(Y, X)
		SWIZZLE_VECTOR2_FUNC(Y, Y)

		inline static Vector2 Lerp(const Vector2& a, const Vector2& b, float t)
		{
			t = Math::Clamp(t, 0.0f, 1.0f);

			Vector2 result;
			result.X = Math::LerpUnclamped(a.X, b.X, t);
			result.Y = Math::LerpUnclamped(a.Y, b.Y, t);
			return result;
		}

		inline static Vector2 LerpUnclamped(const Vector2& a, const Vector2& b, float t)
		{
			Vector2 result;
			result.X = Math::LerpUnclamped(a.X, b.X, t);
			result.Y = Math::LerpUnclamped(a.Y, b.Y, t);
			return result;
		}

		inline static bool EpsilonEqual(const Vector2& a, const Vector2& b, float epsilon = std::numeric_limits<float>::epsilon())
		{
			return Math::EpsilonEqual(a.X, b.X, epsilon) && 
				   Math::EpsilonEqual(a.Y, b.Y, epsilon);
		}

		inline static bool EpsilonNotEqual(const Vector2& a, const Vector2& b, float epsilon = std::numeric_limits<float>::epsilon())
		{
			return Math::EpsilonNotEqual(a.X, b.X, epsilon) || 
				   Math::EpsilonNotEqual(a.Y, b.Y, epsilon);
		}

		float LengthSquared() const
		{
			return X * X + Y * Y;
		}

		float Length() const
		{
			return Math::Sqrt(LengthSquared());
		}

		Vector2& Normalize()
		{
			float invLength = 1.0f / Length();
			X *= invLength;
			Y *= invLength;
			return *this;
		}

		Vector2 operator-() const
		{
			return { -X, -Y };
		}

		Vector2 operator+(const Vector2& v) const
		{
			Vector2 result;
			result.X = X + v.X;
			result.Y = Y + v.Y;
			return result;
		}

		Vector2& operator+=(const Vector2& v)
		{
			X += v.X;
			Y += v.Y;
			return *this;
		}

		Vector2 operator-(const Vector2& v) const
		{
			Vector2 result;
			result.X = X - v.X;
			result.Y = Y - v.Y;
			return result;
		}

		Vector2& operator-=(const Vector2& v)
		{
			X -= v.X;
			Y -= v.Y;
			return *this;
		}

		Vector2 operator*(const Vector2& v) const
		{
			Vector2 result;
			result.X = X * v.X;
			result.Y = Y * v.Y;
			return result;
		}

		Vector2 operator*(float scalar) const
		{
			Vector2 result;
			result.X = X * scalar;
			result.Y = Y * scalar;
			return result;
		}

		Vector2& operator*=(float scalar)
		{
			X *= scalar;
			Y *= scalar;
			return *this;
		}

		Vector2 operator/(const Vector2& v) const
		{
			Vector2 result;
			result.X = X / v.X;
			result.Y = Y / v.Y;
			return result;
		}

		Vector2 operator/(float scalar) const
		{
			Vector2 result;
			result.X = X / scalar;
			result.Y = Y / scalar;
			return result;
		}

		float& operator[](uint32 index)
		{
			switch (index)
			{
			case 0: return X;
			case 1: return Y;
			}
			FLUX_VERIFY(false, "Invalid Vector2 index!");
			return X;
		}

		const float& operator[](uint32 index) const
		{
			switch (index)
			{
			case 0: return X;
			case 1: return Y;
			}
			FLUX_VERIFY(false, "Invalid Vector2 index!");
			return X;
		}

		bool operator==(const Vector2& other) const
		{
			return X == other.X && Y == other.Y;
		}

		bool operator!=(const Vector2& other)
		{
			return !(*this == other);
		}

		float* GetPointer() { return &X; }
		const float* GetPointer() const { return &X; }
	};

}