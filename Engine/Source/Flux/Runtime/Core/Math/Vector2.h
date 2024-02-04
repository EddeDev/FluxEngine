#pragma once

#include "MathUtils.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Vector2
	{
		float X, Y;

		Vector2()
#if 0
			: X(0.0f), Y(0.0f)
#endif
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

		Vector2 operator/(const Vector2& v)
		{
			Vector2 result;
			result.X = X / v.X;
			result.Y = Y / v.Y;
			return result;
		}

		Vector2 operator/(float scalar)
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

		const float* GetFloatPointer() const { return &X; }
	};

}