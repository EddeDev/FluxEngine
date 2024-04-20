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

		Vector4(const Vector4& other)
			: X(other.X), Y(other.Y), Z(other.Z), W(other.W)
		{
		}

		struct Vector4Wrapper
		{
			float& X;
			float& Y;
			float& Z;
			float& W;

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

			Vector4Wrapper& operator+=(const Vector4& v)
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

			Vector4Wrapper& operator-=(const Vector4& v)
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

			Vector4Wrapper& operator*=(float scalar)
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

			Vector4Wrapper& operator/=(float scalar)
			{
				X /= scalar;
				Y /= scalar;
				Z /= scalar;
				W /= scalar;
				return *this;
			}

			operator Vector4() const
			{
				return { X, Y, Z, W };
			}
		};

		struct Vector4WrapperConst
		{
			const float& X;
			const float& Y;
			const float& Z;
			const float& W;

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

			Vector4 operator-(const Vector4& v) const
			{
				Vector4 result;
				result.X = X - v.X;
				result.Y = Y - v.Y;
				result.Z = Z - v.Z;
				result.W = W - v.W;
				return result;
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
		};

#define SWIZZLE_VECTOR4_FUNC(x, y, z, w, ...) \
Vector4Wrapper __VA_ARGS__ ##x##y##z##w() { return { x, y, z, w }; } \
Vector4WrapperConst __VA_ARGS__ ##x##y##z##w() const { return { x, y, z, w }; }

		SWIZZLE_VECTOR4_FUNC(X, X, X, X)
		SWIZZLE_VECTOR4_FUNC(X, X, X, Y)
		SWIZZLE_VECTOR4_FUNC(X, X, X, Z)
		SWIZZLE_VECTOR4_FUNC(X, X, X, W)
		SWIZZLE_VECTOR4_FUNC(X, X, Y, X)
		SWIZZLE_VECTOR4_FUNC(X, X, Y, Y)
		SWIZZLE_VECTOR4_FUNC(X, X, Y, Z)
		SWIZZLE_VECTOR4_FUNC(X, X, Y, W)
		SWIZZLE_VECTOR4_FUNC(X, X, Z, X)
		SWIZZLE_VECTOR4_FUNC(X, X, Z, Y)
		SWIZZLE_VECTOR4_FUNC(X, X, Z, Z)
		SWIZZLE_VECTOR4_FUNC(X, X, Z, W)
		SWIZZLE_VECTOR4_FUNC(X, X, W, X)
		SWIZZLE_VECTOR4_FUNC(X, X, W, Y)
		SWIZZLE_VECTOR4_FUNC(X, X, W, Z)
		SWIZZLE_VECTOR4_FUNC(X, X, W, W)
		SWIZZLE_VECTOR4_FUNC(X, Y, X, X)
		SWIZZLE_VECTOR4_FUNC(X, Y, X, Y)
		SWIZZLE_VECTOR4_FUNC(X, Y, X, Z)
		SWIZZLE_VECTOR4_FUNC(X, Y, X, W)
		SWIZZLE_VECTOR4_FUNC(X, Y, Y, X)
		SWIZZLE_VECTOR4_FUNC(X, Y, Y, Y)
		SWIZZLE_VECTOR4_FUNC(X, Y, Y, Z)
		SWIZZLE_VECTOR4_FUNC(X, Y, Y, W)
		SWIZZLE_VECTOR4_FUNC(X, Y, Z, X)
		SWIZZLE_VECTOR4_FUNC(X, Y, Z, Y)
		SWIZZLE_VECTOR4_FUNC(X, Y, Z, Z)
		SWIZZLE_VECTOR4_FUNC(X, Y, Z, W)
		SWIZZLE_VECTOR4_FUNC(X, Y, W, X)
		SWIZZLE_VECTOR4_FUNC(X, Y, W, Y)
		SWIZZLE_VECTOR4_FUNC(X, Y, W, Z)
		SWIZZLE_VECTOR4_FUNC(X, Y, W, W)
		SWIZZLE_VECTOR4_FUNC(X, Z, X, X)
		SWIZZLE_VECTOR4_FUNC(X, Z, X, Y)
		SWIZZLE_VECTOR4_FUNC(X, Z, X, Z)
		SWIZZLE_VECTOR4_FUNC(X, Z, X, W)
		SWIZZLE_VECTOR4_FUNC(X, Z, Y, X)
		SWIZZLE_VECTOR4_FUNC(X, Z, Y, Y)
		SWIZZLE_VECTOR4_FUNC(X, Z, Y, Z)
		SWIZZLE_VECTOR4_FUNC(X, Z, Y, W)
		SWIZZLE_VECTOR4_FUNC(X, Z, Z, X)
		SWIZZLE_VECTOR4_FUNC(X, Z, Z, Y)
		SWIZZLE_VECTOR4_FUNC(X, Z, Z, Z)
		SWIZZLE_VECTOR4_FUNC(X, Z, Z, W)
		SWIZZLE_VECTOR4_FUNC(X, Z, W, X)
		SWIZZLE_VECTOR4_FUNC(X, Z, W, Y)
		SWIZZLE_VECTOR4_FUNC(X, Z, W, Z)
		SWIZZLE_VECTOR4_FUNC(X, Z, W, W)
		SWIZZLE_VECTOR4_FUNC(X, W, X, X)
		SWIZZLE_VECTOR4_FUNC(X, W, X, Y)
		SWIZZLE_VECTOR4_FUNC(X, W, X, Z)
		SWIZZLE_VECTOR4_FUNC(X, W, X, W)
		SWIZZLE_VECTOR4_FUNC(X, W, Y, X)
		SWIZZLE_VECTOR4_FUNC(X, W, Y, Y)
		SWIZZLE_VECTOR4_FUNC(X, W, Y, Z)
		SWIZZLE_VECTOR4_FUNC(X, W, Y, W)
		SWIZZLE_VECTOR4_FUNC(X, W, Z, X)
		SWIZZLE_VECTOR4_FUNC(X, W, Z, Y)
		SWIZZLE_VECTOR4_FUNC(X, W, Z, Z)
		SWIZZLE_VECTOR4_FUNC(X, W, Z, W)
		SWIZZLE_VECTOR4_FUNC(X, W, W, X)
		SWIZZLE_VECTOR4_FUNC(X, W, W, Y)
		SWIZZLE_VECTOR4_FUNC(X, W, W, Z)
		SWIZZLE_VECTOR4_FUNC(X, W, W, W)
		SWIZZLE_VECTOR4_FUNC(Y, X, X, X)
		SWIZZLE_VECTOR4_FUNC(Y, X, X, Y)
		SWIZZLE_VECTOR4_FUNC(Y, X, X, Z)
		SWIZZLE_VECTOR4_FUNC(Y, X, X, W)
		SWIZZLE_VECTOR4_FUNC(Y, X, Y, X)
		SWIZZLE_VECTOR4_FUNC(Y, X, Y, Y)
		SWIZZLE_VECTOR4_FUNC(Y, X, Y, Z)
		SWIZZLE_VECTOR4_FUNC(Y, X, Y, W)
		SWIZZLE_VECTOR4_FUNC(Y, X, Z, X)
		SWIZZLE_VECTOR4_FUNC(Y, X, Z, Y)
		SWIZZLE_VECTOR4_FUNC(Y, X, Z, Z)
		SWIZZLE_VECTOR4_FUNC(Y, X, Z, W)
		SWIZZLE_VECTOR4_FUNC(Y, X, W, X)
		SWIZZLE_VECTOR4_FUNC(Y, X, W, Y)
		SWIZZLE_VECTOR4_FUNC(Y, X, W, Z)
		SWIZZLE_VECTOR4_FUNC(Y, X, W, W)
		SWIZZLE_VECTOR4_FUNC(Y, Y, X, X)
		SWIZZLE_VECTOR4_FUNC(Y, Y, X, Y)
		SWIZZLE_VECTOR4_FUNC(Y, Y, X, Z)
		SWIZZLE_VECTOR4_FUNC(Y, Y, X, W)
		SWIZZLE_VECTOR4_FUNC(Y, Y, Y, X)
		SWIZZLE_VECTOR4_FUNC(Y, Y, Y, Y)
		SWIZZLE_VECTOR4_FUNC(Y, Y, Y, Z)
		SWIZZLE_VECTOR4_FUNC(Y, Y, Y, W)
		SWIZZLE_VECTOR4_FUNC(Y, Y, Z, X)
		SWIZZLE_VECTOR4_FUNC(Y, Y, Z, Y)
		SWIZZLE_VECTOR4_FUNC(Y, Y, Z, Z)
		SWIZZLE_VECTOR4_FUNC(Y, Y, Z, W)
		SWIZZLE_VECTOR4_FUNC(Y, Y, W, X)
		SWIZZLE_VECTOR4_FUNC(Y, Y, W, Y)
		SWIZZLE_VECTOR4_FUNC(Y, Y, W, Z)
		SWIZZLE_VECTOR4_FUNC(Y, Y, W, W)
		SWIZZLE_VECTOR4_FUNC(Y, Z, X, X)
		SWIZZLE_VECTOR4_FUNC(Y, Z, X, Y)
		SWIZZLE_VECTOR4_FUNC(Y, Z, X, Z)
		SWIZZLE_VECTOR4_FUNC(Y, Z, X, W)
		SWIZZLE_VECTOR4_FUNC(Y, Z, Y, X)
		SWIZZLE_VECTOR4_FUNC(Y, Z, Y, Y)
		SWIZZLE_VECTOR4_FUNC(Y, Z, Y, Z)
		SWIZZLE_VECTOR4_FUNC(Y, Z, Y, W)
		SWIZZLE_VECTOR4_FUNC(Y, Z, Z, X)
		SWIZZLE_VECTOR4_FUNC(Y, Z, Z, Y)
		SWIZZLE_VECTOR4_FUNC(Y, Z, Z, Z)
		SWIZZLE_VECTOR4_FUNC(Y, Z, Z, W)
		SWIZZLE_VECTOR4_FUNC(Y, Z, W, X)
		SWIZZLE_VECTOR4_FUNC(Y, Z, W, Y)
		SWIZZLE_VECTOR4_FUNC(Y, Z, W, Z)
		SWIZZLE_VECTOR4_FUNC(Y, Z, W, W)
		SWIZZLE_VECTOR4_FUNC(Y, W, X, X)
		SWIZZLE_VECTOR4_FUNC(Y, W, X, Y)
		SWIZZLE_VECTOR4_FUNC(Y, W, X, Z)
		SWIZZLE_VECTOR4_FUNC(Y, W, X, W)
		SWIZZLE_VECTOR4_FUNC(Y, W, Y, X)
		SWIZZLE_VECTOR4_FUNC(Y, W, Y, Y)
		SWIZZLE_VECTOR4_FUNC(Y, W, Y, Z)
		SWIZZLE_VECTOR4_FUNC(Y, W, Y, W)
		SWIZZLE_VECTOR4_FUNC(Y, W, Z, X)
		SWIZZLE_VECTOR4_FUNC(Y, W, Z, Y)
		SWIZZLE_VECTOR4_FUNC(Y, W, Z, Z)
		SWIZZLE_VECTOR4_FUNC(Y, W, Z, W)
		SWIZZLE_VECTOR4_FUNC(Y, W, W, X)
		SWIZZLE_VECTOR4_FUNC(Y, W, W, Y)
		SWIZZLE_VECTOR4_FUNC(Y, W, W, Z)
		SWIZZLE_VECTOR4_FUNC(Y, W, W, W)
		SWIZZLE_VECTOR4_FUNC(Z, X, X, X)
		SWIZZLE_VECTOR4_FUNC(Z, X, X, Y)
		SWIZZLE_VECTOR4_FUNC(Z, X, X, Z)
		SWIZZLE_VECTOR4_FUNC(Z, X, X, W)
		SWIZZLE_VECTOR4_FUNC(Z, X, Y, X)
		SWIZZLE_VECTOR4_FUNC(Z, X, Y, Y)
		SWIZZLE_VECTOR4_FUNC(Z, X, Y, Z)
		SWIZZLE_VECTOR4_FUNC(Z, X, Y, W)
		SWIZZLE_VECTOR4_FUNC(Z, X, Z, X)
		SWIZZLE_VECTOR4_FUNC(Z, X, Z, Y)
		SWIZZLE_VECTOR4_FUNC(Z, X, Z, Z)
		SWIZZLE_VECTOR4_FUNC(Z, X, Z, W)
		SWIZZLE_VECTOR4_FUNC(Z, X, W, X)
		SWIZZLE_VECTOR4_FUNC(Z, X, W, Y)
		SWIZZLE_VECTOR4_FUNC(Z, X, W, Z)
		SWIZZLE_VECTOR4_FUNC(Z, X, W, W)
		SWIZZLE_VECTOR4_FUNC(Z, Y, X, X)
		SWIZZLE_VECTOR4_FUNC(Z, Y, X, Y)
		SWIZZLE_VECTOR4_FUNC(Z, Y, X, Z)
		SWIZZLE_VECTOR4_FUNC(Z, Y, X, W)
		SWIZZLE_VECTOR4_FUNC(Z, Y, Y, X)
		SWIZZLE_VECTOR4_FUNC(Z, Y, Y, Y)
		SWIZZLE_VECTOR4_FUNC(Z, Y, Y, Z)
		SWIZZLE_VECTOR4_FUNC(Z, Y, Y, W)
		SWIZZLE_VECTOR4_FUNC(Z, Y, Z, X)
		SWIZZLE_VECTOR4_FUNC(Z, Y, Z, Y)
		SWIZZLE_VECTOR4_FUNC(Z, Y, Z, Z)
		SWIZZLE_VECTOR4_FUNC(Z, Y, Z, W)
		SWIZZLE_VECTOR4_FUNC(Z, Y, W, X)
		SWIZZLE_VECTOR4_FUNC(Z, Y, W, Y)
		SWIZZLE_VECTOR4_FUNC(Z, Y, W, Z)
		SWIZZLE_VECTOR4_FUNC(Z, Y, W, W)
		SWIZZLE_VECTOR4_FUNC(Z, Z, X, X)
		SWIZZLE_VECTOR4_FUNC(Z, Z, X, Y)
		SWIZZLE_VECTOR4_FUNC(Z, Z, X, Z)
		SWIZZLE_VECTOR4_FUNC(Z, Z, X, W)
		SWIZZLE_VECTOR4_FUNC(Z, Z, Y, X)
		SWIZZLE_VECTOR4_FUNC(Z, Z, Y, Y)
		SWIZZLE_VECTOR4_FUNC(Z, Z, Y, Z)
		SWIZZLE_VECTOR4_FUNC(Z, Z, Y, W)
		SWIZZLE_VECTOR4_FUNC(Z, Z, Z, X)
		SWIZZLE_VECTOR4_FUNC(Z, Z, Z, Y)
		SWIZZLE_VECTOR4_FUNC(Z, Z, Z, Z)
		SWIZZLE_VECTOR4_FUNC(Z, Z, Z, W)
		SWIZZLE_VECTOR4_FUNC(Z, Z, W, X)
		SWIZZLE_VECTOR4_FUNC(Z, Z, W, Y)
		SWIZZLE_VECTOR4_FUNC(Z, Z, W, Z)
		SWIZZLE_VECTOR4_FUNC(Z, Z, W, W)
		SWIZZLE_VECTOR4_FUNC(Z, W, X, X)
		SWIZZLE_VECTOR4_FUNC(Z, W, X, Y)
		SWIZZLE_VECTOR4_FUNC(Z, W, X, Z)
		SWIZZLE_VECTOR4_FUNC(Z, W, X, W)
		SWIZZLE_VECTOR4_FUNC(Z, W, Y, X)
		SWIZZLE_VECTOR4_FUNC(Z, W, Y, Y)
		SWIZZLE_VECTOR4_FUNC(Z, W, Y, Z)
		SWIZZLE_VECTOR4_FUNC(Z, W, Y, W)
		SWIZZLE_VECTOR4_FUNC(Z, W, Z, X)
		SWIZZLE_VECTOR4_FUNC(Z, W, Z, Y)
		SWIZZLE_VECTOR4_FUNC(Z, W, Z, Z)
		SWIZZLE_VECTOR4_FUNC(Z, W, Z, W)
		SWIZZLE_VECTOR4_FUNC(Z, W, W, X)
		SWIZZLE_VECTOR4_FUNC(Z, W, W, Y)
		SWIZZLE_VECTOR4_FUNC(Z, W, W, Z)
		SWIZZLE_VECTOR4_FUNC(Z, W, W, W)
		SWIZZLE_VECTOR4_FUNC(W, X, X, X)
		SWIZZLE_VECTOR4_FUNC(W, X, X, Y)
		SWIZZLE_VECTOR4_FUNC(W, X, X, Z)
		SWIZZLE_VECTOR4_FUNC(W, X, X, W)
		SWIZZLE_VECTOR4_FUNC(W, X, Y, X)
		SWIZZLE_VECTOR4_FUNC(W, X, Y, Y)
		SWIZZLE_VECTOR4_FUNC(W, X, Y, Z)
		SWIZZLE_VECTOR4_FUNC(W, X, Y, W)
		SWIZZLE_VECTOR4_FUNC(W, X, Z, X)
		SWIZZLE_VECTOR4_FUNC(W, X, Z, Y)
		SWIZZLE_VECTOR4_FUNC(W, X, Z, Z)
		SWIZZLE_VECTOR4_FUNC(W, X, Z, W)
		SWIZZLE_VECTOR4_FUNC(W, X, W, X)
		SWIZZLE_VECTOR4_FUNC(W, X, W, Y)
		SWIZZLE_VECTOR4_FUNC(W, X, W, Z)
		SWIZZLE_VECTOR4_FUNC(W, X, W, W)
		SWIZZLE_VECTOR4_FUNC(W, Y, X, X)
		SWIZZLE_VECTOR4_FUNC(W, Y, X, Y)
		SWIZZLE_VECTOR4_FUNC(W, Y, X, Z)
		SWIZZLE_VECTOR4_FUNC(W, Y, X, W)
		SWIZZLE_VECTOR4_FUNC(W, Y, Y, X)
		SWIZZLE_VECTOR4_FUNC(W, Y, Y, Y)
		SWIZZLE_VECTOR4_FUNC(W, Y, Y, Z)
		SWIZZLE_VECTOR4_FUNC(W, Y, Y, W)
		SWIZZLE_VECTOR4_FUNC(W, Y, Z, X)
		SWIZZLE_VECTOR4_FUNC(W, Y, Z, Y)
		SWIZZLE_VECTOR4_FUNC(W, Y, Z, Z)
		SWIZZLE_VECTOR4_FUNC(W, Y, Z, W)
		SWIZZLE_VECTOR4_FUNC(W, Y, W, X)
		SWIZZLE_VECTOR4_FUNC(W, Y, W, Y)
		SWIZZLE_VECTOR4_FUNC(W, Y, W, Z)
		SWIZZLE_VECTOR4_FUNC(W, Y, W, W)
		SWIZZLE_VECTOR4_FUNC(W, Z, X, X)
		SWIZZLE_VECTOR4_FUNC(W, Z, X, Y)
		SWIZZLE_VECTOR4_FUNC(W, Z, X, Z)
		SWIZZLE_VECTOR4_FUNC(W, Z, X, W)
		SWIZZLE_VECTOR4_FUNC(W, Z, Y, X)
		SWIZZLE_VECTOR4_FUNC(W, Z, Y, Y)
		SWIZZLE_VECTOR4_FUNC(W, Z, Y, Z)
		SWIZZLE_VECTOR4_FUNC(W, Z, Y, W)
		SWIZZLE_VECTOR4_FUNC(W, Z, Z, X)
		SWIZZLE_VECTOR4_FUNC(W, Z, Z, Y)
		SWIZZLE_VECTOR4_FUNC(W, Z, Z, Z)
		SWIZZLE_VECTOR4_FUNC(W, Z, Z, W)
		SWIZZLE_VECTOR4_FUNC(W, Z, W, X)
		SWIZZLE_VECTOR4_FUNC(W, Z, W, Y)
		SWIZZLE_VECTOR4_FUNC(W, Z, W, Z)
		SWIZZLE_VECTOR4_FUNC(W, Z, W, W)
		SWIZZLE_VECTOR4_FUNC(W, W, X, X)
		SWIZZLE_VECTOR4_FUNC(W, W, X, Y)
		SWIZZLE_VECTOR4_FUNC(W, W, X, Z)
		SWIZZLE_VECTOR4_FUNC(W, W, X, W)
		SWIZZLE_VECTOR4_FUNC(W, W, Y, X)
		SWIZZLE_VECTOR4_FUNC(W, W, Y, Y)
		SWIZZLE_VECTOR4_FUNC(W, W, Y, Z)
		SWIZZLE_VECTOR4_FUNC(W, W, Y, W)
		SWIZZLE_VECTOR4_FUNC(W, W, Z, X)
		SWIZZLE_VECTOR4_FUNC(W, W, Z, Y)
		SWIZZLE_VECTOR4_FUNC(W, W, Z, Z)
		SWIZZLE_VECTOR4_FUNC(W, W, Z, W)
		SWIZZLE_VECTOR4_FUNC(W, W, W, X)
		SWIZZLE_VECTOR4_FUNC(W, W, W, Y)
		SWIZZLE_VECTOR4_FUNC(W, W, W, Z)
		SWIZZLE_VECTOR4_FUNC(W, W, W, W)

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

		// TODO: constructor
		Vector3 ToVector3() const
		{
			return { X, Y, Z };
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

		bool operator==(const Vector4& other) const
		{
			return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
		}

		bool operator!=(const Vector4& other)
		{
			return !(*this == other);
		}

		float* GetPointer() { return &X; }
		const float* GetPointer() const { return &X; }
	};

}