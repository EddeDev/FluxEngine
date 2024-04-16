#pragma once

#include "Vector2.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Vector3
	{
	public:
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

		struct Vector3Wrapper
		{
			float& X;
			float& Y;
			float& Z;

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

			Vector3Wrapper& operator+=(const Vector3& v)
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

			Vector3Wrapper& operator-=(const Vector3& v)
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

			Vector3Wrapper& operator*=(float scalar)
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

			Vector3Wrapper& operator/=(float scalar)
			{
				X /= scalar;
				Y /= scalar;
				Z /= scalar;
				return *this;
			}

			operator Vector3() const
			{
				return { X, Y, Z };
			}
		};

		struct Vector3WrapperConst
		{
			const float& X;
			const float& Y;
			const float& Z;

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

			Vector3 operator-(const Vector3& v) const
			{
				Vector3 result;
				result.X = X - v.X;
				result.Y = Y - v.Y;
				result.Z = Z - v.Z;
				return result;
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
		};

#define SWIZZLE_VECTOR3_FUNC(x, y, z, ...) \
Vector3Wrapper __VA_ARGS__ ##x##y##z() { return { x, y, z }; } \
Vector3WrapperConst __VA_ARGS__ ##x##y##z() const { return { x, y, z }; }

		SWIZZLE_VECTOR3_FUNC(X, X, X)
		SWIZZLE_VECTOR3_FUNC(X, X, Y)
		SWIZZLE_VECTOR3_FUNC(X, X, Z)
		SWIZZLE_VECTOR3_FUNC(X, Y, X)
		SWIZZLE_VECTOR3_FUNC(X, Y, Y)
		SWIZZLE_VECTOR3_FUNC(X, Y, Z)
		SWIZZLE_VECTOR3_FUNC(X, Z, X)
		SWIZZLE_VECTOR3_FUNC(X, Z, Y)
		SWIZZLE_VECTOR3_FUNC(X, Z, Z)
		SWIZZLE_VECTOR3_FUNC(Y, X, X)
		SWIZZLE_VECTOR3_FUNC(Y, X, Y)
		SWIZZLE_VECTOR3_FUNC(Y, X, Z)
		SWIZZLE_VECTOR3_FUNC(Y, Y, X)
		SWIZZLE_VECTOR3_FUNC(Y, Y, Y)
		SWIZZLE_VECTOR3_FUNC(Y, Y, Z)
		SWIZZLE_VECTOR3_FUNC(Y, Z, X)
		SWIZZLE_VECTOR3_FUNC(Y, Z, Y)
		SWIZZLE_VECTOR3_FUNC(Y, Z, Z)
		SWIZZLE_VECTOR3_FUNC(Z, X, X)
		SWIZZLE_VECTOR3_FUNC(Z, X, Y)
		SWIZZLE_VECTOR3_FUNC(Z, X, Z)
		SWIZZLE_VECTOR3_FUNC(Z, Y, X)
		SWIZZLE_VECTOR3_FUNC(Z, Y, Y)
		SWIZZLE_VECTOR3_FUNC(Z, Y, Z)
		SWIZZLE_VECTOR3_FUNC(Z, Z, X)
		SWIZZLE_VECTOR3_FUNC(Z, Z, Y)
		SWIZZLE_VECTOR3_FUNC(Z, Z, Z)

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

		Vector3 Normalized() const
		{
			Vector3 result;
			float invLength = 1.0f / Length();
			result.X = X * invLength;
			result.Y = Y * invLength;
			result.Z = Z * invLength;
			return result;
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