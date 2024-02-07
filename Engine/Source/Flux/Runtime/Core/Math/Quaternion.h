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
			W = (cosX * cosY * cosZ) - (sinX * sinY * sinZ);
		}

		float GetPitch() const
		{
			float y = 2.0f * (Y * Z + W * X);
			float x = (W * W) - (X * X) - (Y * Y) + (Z * Z);

			if (Math::Abs(x) < std::numeric_limits<float>::epsilon() && Math::Abs(y) < std::numeric_limits<float>::epsilon())
				return 2.0f * Math::Atan2 (X, W);

			return Math::Atan2(y, x);
		}

		float GetYaw() const
		{
			return Math::Asin(Math::Clamp(-2.0f * (X * Z - W * Y), -1.0f, 1.0f));
		}

		float GetRoll() const
		{
			float y = 2.0f * (X * Y + W * Z);
			float x = (W * W) - (X * X) - (Y * Y) + (Z * Z);

			if (Math::Abs(x) < std::numeric_limits<float>::epsilon() && Math::Abs(y) < std::numeric_limits<float>::epsilon())
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

		Vector3 operator*(const Vector3& v) const
		{
			Vector3 a(X, Y, Z);
			Vector3 b(Vector3::Cross(a, v));

			return v + ((b * W) + Vector3::Cross(a, b)) * 2.0f;
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

		const float* GetPointer() const { return &X; }
	};

}