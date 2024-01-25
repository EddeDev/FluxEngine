#pragma once

#include "MathUtils.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

namespace Flux {

	struct Quaternion
	{
		float X, Y, Z, W;

		Quaternion()
			: X(0.0f), Y(0.0f), Z(0.0f), W(1.0f)
		{
		}

		Quaternion(const Vector3& eulerAngles)
		{
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

			result.A1 = 1.0f - 2.0f * (yy + zz);
			result.B1 = 2.0f * (xy + wz);
			result.C1 = 2.0f * (xz - wy);

			result.A2 = 2.0f * (xy - wz);
			result.B2 = 1.0f - 2.0f * (xx + zz);
			result.C2 = 2.0f * (yz + wx);

			result.A3 = 2.0f * (xz + wy);
			result.B3 = 2.0f * (yz - wx);
			result.C3 = 1.0f - 2.0f * (xx + yy);

			return result;
		}

		Matrix4x4 ToMatrix4x4() const
		{
			return Matrix4x4(ToMatrix3x3());
		}
	};

}