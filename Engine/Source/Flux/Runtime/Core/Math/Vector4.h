#pragma once

#include "Vector3.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Vector4
	{
		union
		{
			struct
			{
				float X, Y, Z, W;
			};
			
			struct
			{
				float R, G, B, A;
			};
		};

		Vector4()
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

		inline static Vector4 Lerp(const Vector4& a, const Vector4& b, float t)
		{
			t = Math::Clamp(t, 0.0f, 1.0f);

			Vector4 result;
			result.X = Math::LerpUnclamped(a.X, b.X, t);
			result.Y = Math::LerpUnclamped(a.Y, b.Y, t);
			result.Z = Math::LerpUnclamped(a.Z, b.Z, t);
			result.W = Math::LerpUnclamped(a.W, b.W, t);
			return result;
		}

		inline static Vector4 LerpUnclamped(const Vector4& a, const Vector4& b, float t)
		{
			Vector4 result;
			result.X = Math::LerpUnclamped(a.X, b.X, t);
			result.Y = Math::LerpUnclamped(a.Y, b.Y, t);
			result.Z = Math::LerpUnclamped(a.Z, b.Z, t);
			result.W = Math::LerpUnclamped(a.W, b.W, t);
			return result;
		}

		inline static bool EpsilonEqual(const Vector4& a, const Vector4& b, float epsilon = std::numeric_limits<float>::epsilon())
		{
			return Math::EpsilonEqual(a.X, b.X, epsilon) &&
				   Math::EpsilonEqual(a.Y, b.Y, epsilon) &&
				   Math::EpsilonEqual(a.Z, b.Z, epsilon) &&
				   Math::EpsilonEqual(a.W, b.W, epsilon);
		}

		inline static bool EpsilonNotEqual(const Vector4& a, const Vector4& b, float epsilon = std::numeric_limits<float>::epsilon())
		{
			return Math::EpsilonNotEqual(a.X, b.X, epsilon) ||
				   Math::EpsilonNotEqual(a.Y, b.Y, epsilon) ||
				   Math::EpsilonNotEqual(a.Z, b.Z, epsilon) ||
				   Math::EpsilonNotEqual(a.W, b.W, epsilon);
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

		Vector4 operator+(const Vector4& v) const
		{
			Vector4 result;
			result.X = X + v.X;
			result.Y = Y + v.Y;
			result.Z = Z + v.Z;
			result.W = W + v.W;
			return result;
		}

		Vector4& operator+=(const Vector4& v)
		{
			X += v.X;
			Y += v.Y;
			Z += v.Z;
			W += v.W;
			return *this;
		}

		Vector4 operator-(const Vector4& v) const
		{
			Vector4 result;
			result.X = X - v.X;
			result.Y = Y - v.Y;
			result.Z = Z - v.Z;
			result.W = W - v.W;
			return result;
		}

		Vector4& operator-=(const Vector4& v)
		{
			X -= v.X;
			Y -= v.Y;
			Z -= v.Z;
			W -= v.W;
			return *this;
		}

		Vector4 operator*(const Vector4& v) const
		{
			Vector4 result;
			result.X = X * v.X;
			result.Y = Y * v.Y;
			result.Z = Z * v.Z;
			result.W = W * v.W;
			return result;
		}

		Vector4 operator*(float scalar) const
		{
			Vector4 result;
			result.X = X * scalar;
			result.Y = Y * scalar;
			result.Z = Z * scalar;
			result.W = W * scalar;
			return result;
		}

		Vector4& operator*=(float scalar)
		{
			X *= scalar;
			Y *= scalar;
			Z *= scalar;
			W *= scalar;
			return *this;
		}

		Vector4 operator/(const Vector4& v) const
		{
			Vector4 result;
			result.X = X / v.X;
			result.Y = Y / v.Y;
			result.Z = Z / v.Z;
			result.W = W / v.W;
			return result;
		}

		Vector4 operator/(float scalar) const
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

		float* GetPointer() { return &X; }
		const float* GetPointer() const { return &X; }
	};

}