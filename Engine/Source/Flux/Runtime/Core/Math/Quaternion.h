#pragma once

#include "MathUtils.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Quaternion
	{
		float X, Y, Z, W;

		Quaternion()
		{
		}

		Quaternion(float x, float y, float z, float w)
			: X(x), Y(y), Z(z), W(w)
		{
		}

		Quaternion(const Vector3& eulerAngles)
		{
			// Based on https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		
			float cosX = Math::Cos(eulerAngles.X * 0.5f);
			float cosY = Math::Cos(eulerAngles.Y * 0.5f);
			float cosZ = Math::Cos(eulerAngles.Z * 0.5f);

			float sinX = Math::Sin(eulerAngles.X * 0.5f);
			float sinY = Math::Sin(eulerAngles.Y * 0.5f);
			float sinZ = Math::Sin(eulerAngles.Z * 0.5f);

			X = (sinX * cosY * cosZ) - (cosX * sinY * sinZ);
			Y = (cosX * sinY * cosZ) + (sinX * cosY * sinZ);
			Z = (cosX * cosY * sinZ) - (sinX * sinY * cosZ);
			W = (cosX * cosY * cosZ) + (sinX * sinY * sinZ);
		}

		inline static Quaternion Conjugate(const Quaternion& q)
		{
			return { -q.X, -q.Y, -q.Z, q.W };
		}

		inline static Quaternion Inverse(const Quaternion& q)
		{
			return Conjugate(q) / q.LengthSquared();
		}

		inline static bool EpsilonEqual(const Quaternion& a, const Quaternion& b, float epsilon = std::numeric_limits<float>::epsilon())
		{
			return Math::EpsilonEqual(a.X, b.X, epsilon) &&
				   Math::EpsilonEqual(a.Y, b.Y, epsilon) &&
				   Math::EpsilonEqual(a.Z, b.Z, epsilon) &&
				   Math::EpsilonEqual(a.W, b.W, epsilon);
		}

		inline static bool EpsilonNotEqual(const Quaternion& a, const Quaternion& b, float epsilon = std::numeric_limits<float>::epsilon())
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

		float GetPitch() const
		{
			float y = 2.0f * (Y * Z + W * X);
			float x = (W * W) - (X * X) - (Y * Y) + (Z * Z);

			if (Vector2::EpsilonEqual({ x, y }, Vector2(0.0f)))
				return 2.0f * Math::Atan2(X, W);

			return Math::Atan2(y, x);
		}

		float GetYaw() const
		{
			return Math::Asin(Math::Clamp(-2.0f * (X * Z - W * Y), -1.0f, 1.0f));
		}

		float GetRoll() const
		{
			float y = 2.0f * (X * Y + W * Z);
			float x = (W * W) + (X * X) - (Y * Y) - (Z * Z);

			if (Vector2::EpsilonEqual({ x, y }, Vector2(0.0f)))
				return 0.0f;

			return Math::Atan2(y, x);
		}

		Vector3 GetEulerAngles() const
		{
			return { GetPitch(), GetYaw(), GetRoll() };
		}

		Matrix3x3 ToMatrix3x3() const
		{
			Matrix3x3 result(1.0f);

			float xx = X * X;
			float yy = Y * Y;
			float zz = Z * Z;

			float xz = X * Z;
			float xy = X * Y;
			float yz = Y * Z;

			float wx = W * X;
			float wy = W * Y;
			float wz = W * Z;

			result[0][0] = 1.0f - 2.0f * (yy + zz);
			result[0][1] = 2.0f * (xy + wz);
			result[0][2] = 2.0f * (xz - wy);

			result[1][0] = 2.0f * (xy - wz);
			result[1][1] = 1.0f - 2.0f * (xx + zz);
			result[1][2] = 2.0f * (yz + wx);

			result[2][0] = 2.0f * (xz + wy);
			result[2][1] = 2.0f * (yz - wx);
			result[2][2] = 1.0f - 2.0f * (xx + yy);

			return result;
		}

		Matrix4x4 ToMatrix4x4() const
		{
			return Matrix4x4(ToMatrix3x3());
		}

		operator Vector4&() const
		{
			return *(Vector4*)this;
		}

		Quaternion operator*(const Quaternion& q) const
		{
			Quaternion result;
			result.X = W * q.X + X * q.W + Y * q.Z - Z * q.Y;
			result.Y = W * q.Y + Y * q.W + Z * q.X - X * q.Z;
			result.Z = W * q.Z + Z * q.W + X * q.Y - Y * q.X;
			result.W = W * q.W - X * q.X - Y * q.Y - Z * q.Z;
			return result;
		}

		Vector3 operator*(const Vector3& v) const
		{
			Vector3 a(X, Y, Z);
			Vector3 b(Vector3::Cross(a, v));

			return v + ((b * W) + Vector3::Cross(a, b)) * 2.0f;
		}

		Quaternion operator/(float scalar) const
		{
			Quaternion result;
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
			FLUX_VERIFY(false, "Invalid Quaternion index!");
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
			FLUX_VERIFY(false, "Invalid Quaternion index!");
			return X;
		}

		float* GetPointer() { return &X; }
		const float* GetPointer() const { return &X; }
	};

}