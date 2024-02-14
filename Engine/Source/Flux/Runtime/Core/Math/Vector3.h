#pragma once

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Vector3
	{
		union
		{
			struct
			{
				float X, Y, Z;
			};

			struct
			{
				float R, G, B;
			};
		};

		Vector3()
		{
		}

		explicit Vector3(float scalar)
			: X(scalar), Y(scalar), Z(scalar)
		{
		}

		Vector3(const Vector2& v, float z)
			: X(v.X), Y(v.Y), Z(z)
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

		inline static float Dot(const Vector3& a, const Vector3& b)
		{
			Vector3 result(a * b);
			return result.X + result.Y + result.Z;
		}

		inline static bool EpsilonEqual(const Vector3& a, const Vector3& b, float epsilon = std::numeric_limits<float>::epsilon())
		{
			return Math::EpsilonEqual(a.X, b.X, epsilon) &&
				   Math::EpsilonEqual(a.Y, b.Y, epsilon) &&
				   Math::EpsilonEqual(a.Z, b.Z, epsilon);
		}

		inline static bool EpsilonNotEqual(const Vector3& a, const Vector3& b, float epsilon = std::numeric_limits<float>::epsilon())
		{
			return Math::EpsilonNotEqual(a.X, b.X, epsilon) ||
				   Math::EpsilonNotEqual(a.Y, b.Y, epsilon) ||
				   Math::EpsilonNotEqual(a.Z, b.Z, epsilon);
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

		Vector3 operator/(const Vector3& v) const
		{
			Vector3 result;
			result.X = X / v.X;
			result.Y = Y / v.Y;
			result.Z = Z / v.Z;
			return result;
		}

		Vector3 operator/(float scalar) const
		{
			Vector3 result;
			result.X = X / scalar;
			result.Y = Y / scalar;
			result.Z = Z / scalar;
			return result;
		}

		Vector3& operator/=(float scalar)
		{
			X /= scalar;
			Y /= scalar;
			Z /= scalar;
			return *this;
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

		float* GetPointer() { return &X; }
		const float* GetPointer() const { return &X; }
	};

}