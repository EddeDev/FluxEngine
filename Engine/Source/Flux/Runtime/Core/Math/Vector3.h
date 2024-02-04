#pragma once

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Vector3
	{
		float X, Y, Z;

		Vector3()
#if 0
			: X(0.0f), Y(0.0f), Z(0.0f)
#endif
		{
		}

		explicit Vector3(float scalar)
			: X(scalar), Y(scalar), Z(scalar)
		{
		}

		Vector3(float x, float y, float z)
			: X(x), Y(y), Z(z)
		{
		}

		inline static Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
		{
			t = Math::Clamp(t, 0.0f, 1.0f);

			Vector3 result;
			result.X = Math::LerpUnclamped(a.X, b.X, t);
			result.Y = Math::LerpUnclamped(a.Y, b.Y, t);
			result.Z = Math::LerpUnclamped(a.Z, b.Z, t);
			return result;
		}

		inline static Vector3 LerpUnclamped(const Vector3& a, const Vector3& b, float t)
		{
			Vector3 result;
			result.X = Math::LerpUnclamped(a.X, b.X, t);
			result.Y = Math::LerpUnclamped(a.Y, b.Y, t);
			result.Z = Math::LerpUnclamped(a.Z, b.Z, t);
			return result;
		}

		inline static Vector3 Cross(const Vector3& a, const Vector3& b)
		{
			Vector3 result;
			result.X = a.Y * b.Z - b.Y * a.Z;
			result.Y = a.Z * b.X - b.Z * a.X;
			result.Z = a.X * b.Y - b.X * a.Y;
			return result;
		}

		float LengthSquared() const
		{
			return X * X + Y * Y + Z * Z;
		}

		float Length() const
		{
			return Math::Sqrt(LengthSquared());
		}

		Vector3& Normalize()
		{
			float invLength = 1.0f / Length();
			X *= invLength;
			Y *= invLength;
			Z *= invLength;
			return *this;
		}

		Vector3 operator-() const
		{
			return { -X, -Y, -Z };
		}

		Vector3 operator+(const Vector3& v) const
		{
			Vector3 result;
			result.X = X + v.X;
			result.Y = Y + v.Y;
			result.Z = Z + v.Z;
			return result;
		}

		Vector3& operator+=(const Vector3& v)
		{
			X += v.X;
			Y += v.Y;
			Z += v.Z;
			return *this;
		}

		Vector3 operator-(const Vector3& v) const
		{
			Vector3 result;
			result.X = X - v.X;
			result.Y = Y - v.Y;
			result.Z = Z - v.Z;
			return result;
		}

		Vector3& operator-=(const Vector3& v)
		{
			X -= v.X;
			Y -= v.Y;
			Z -= v.Z;
			return *this;
		}

		Vector3 operator*(const Vector3& v) const
		{
			Vector3 result;
			result.X = X * v.X;
			result.Y = Y * v.Y;
			result.Z = Z * v.Z;
			return result;
		}

		Vector3 operator*(float scalar) const
		{
			Vector3 result;
			result.X = X * scalar;
			result.Y = Y * scalar;
			result.Z = Z * scalar;
			return result;
		}

		Vector3& operator*=(float scalar)
		{
			X *= scalar;
			Y *= scalar;
			Z *= scalar;
			return *this;
		}

		Vector3 operator/(const Vector3& v)
		{
			Vector3 result;
			result.X = X / v.X;
			result.Y = Y / v.Y;
			result.Z = Z / v.Z;
			return result;
		}

		Vector3 operator/(float scalar)
		{
			Vector3 result;
			result.X = X / scalar;
			result.Y = Y / scalar;
			result.Z = Z / scalar;
			return result;
		}

		float& operator[](uint32 index)
		{
			switch (index)
			{
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			}
			FLUX_VERIFY(false, "Invalid Vector3 index!");
			return X;
		}

		float operator[](uint32 index) const
		{
			switch (index)
			{
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			}
			FLUX_VERIFY(false, "Invalid Vector3 index!");
			return X;
		}

		const float* GetFloatPointer() const { return &X; }
	};

}