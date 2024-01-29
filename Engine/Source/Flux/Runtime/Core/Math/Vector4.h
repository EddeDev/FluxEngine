#pragma once

#include "Vector3.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Vector4
	{
		float X, Y, Z, W;

		Vector4()
			: X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
		{
		}

		explicit Vector4(float scalar)
			: X(scalar), Y(scalar), Z(scalar), W(scalar)
		{
		}

		Vector4(const Vector3& v, float w)
			: X(v.X), Y(v.Y), Z(v.Z), W(w)
		{
		}

		Vector4(float x, float y, float z, float w)
			: X(x), Y(y), Z(z), W(w)
		{
		}

		float LengthSquared() const
		{
			return X * X + Y * Y + Z * Z + W * W;
		}

		float Length() const
		{
			return Math::Sqrt(LengthSquared());
		}

		Vector4& Normalize()
		{
			float invLength = 1.0f / Length();
			X *= invLength;
			Y *= invLength;
			Z *= invLength;
			W *= invLength;
			return *this;
		}

		Vector4 operator-() const
		{
			return { -X, -Y, -Z, -W };
		}

		Vector4 operator+(const Vector4& v)
		{
			Vector4 result;
			result.X = X + v.X;
			result.Y = Y + v.Y;
			result.Z = Z + v.Z;
			result.W = W + v.W;
			return result;
		}

		Vector4 operator-(const Vector4& v)
		{
			Vector4 result;
			result.X = X - v.X;
			result.Y = Y - v.Y;
			result.Z = Z - v.Z;
			result.W = W - v.W;
			return result;
		}

		Vector4 operator*(const Vector4& v)
		{
			Vector4 result;
			result.X = X * v.X;
			result.Y = Y * v.Y;
			result.Z = Z * v.Z;
			result.W = W * v.W;
			return result;
		}

		Vector4 operator*(float scalar)
		{
			Vector4 result;
			result.X = X * scalar;
			result.Y = Y * scalar;
			result.Z = Z * scalar;
			result.W = W * scalar;
			return result;
		}

		Vector4 operator/(const Vector4& v)
		{
			Vector4 result;
			result.X = X / v.X;
			result.Y = Y / v.Y;
			result.Z = Z / v.Z;
			result.W = W / v.W;
			return result;
		}

		Vector4 operator/(float scalar)
		{
			Vector4 result;
			result.X = X / scalar;
			result.Y = Y / scalar;
			result.Z = Z / scalar;
			result.W = W / scalar;
			return result;
		}

		float& operator[](uint32 index)
		{
			switch (index)
			{
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			case 3: return W;
			}
			FLUX_VERIFY(false, "Invalid Vector4 index!");
			return X;
		}

		float operator[](uint32 index) const
		{
			switch (index)
			{
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			case 3: return W;
			}
			FLUX_VERIFY(false, "Invalid Vector4 index!");
			return X;
		}

		const float* GetFloatPointer() const { return &X; }
	};

}