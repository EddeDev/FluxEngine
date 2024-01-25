#pragma once

#include "Vector3.h"

namespace Flux {

	struct Matrix3x3
	{
		float A1, B1, C1;
		float A2, B2, C2;
		float A3, B3, C3;

		Matrix3x3()
			: A1(0.0f), A2(0.0f), A3(0.0f),
			B1(0.0f), B2(0.0f), B3(0.0f),
			C1(0.0f), C2(0.0f), C3(0.0f)
		{
		}

		explicit Matrix3x3(float scalar)
		{
			A1 = scalar;
			A2 = 0.0f;
			A3 = 0.0f;

			B1 = 0.0f;
			B2 = scalar;
			B3 = 0.0f;

			C1 = 0.0f;
			C2 = 0.0f;
			C3 = scalar;
		}

		Matrix3x3& SetIdentity()
		{
			A1 = 1.0f;
			A2 = 0.0f;
			A3 = 0.0f;

			B1 = 0.0f;
			B2 = 1.0f;
			B3 = 0.0f;

			C1 = 0.0f;
			C2 = 0.0f;
			C3 = 1.0f;

			return *this;
		}

		Matrix3x3& Scale(const Vector3& scale)
		{
			A1 *= scale.X;
			B2 *= scale.Y;
			C3 *= scale.Z;

			return *this;
		}

		Matrix3x3 Mul(const Matrix3x3& m) const
		{
			Matrix3x3 result;

			result.A1 = A1 * m.A1 + A2 * m.B1 + A3 * m.C1;
			result.A2 = A1 * m.A2 + A2 * m.B2 + A3 * m.C2;
			result.A3 = A1 * m.A3 + A2 * m.B3 + A3 * m.C3;

			result.B1 = B1 * m.A1 + B2 * m.B1 + B3 * m.C1;
			result.B2 = B1 * m.A2 + B2 * m.B2 + B3 * m.C2;
			result.B3 = B1 * m.A3 + B2 * m.B3 + B3 * m.C3;

			result.C1 = C1 * m.A1 + C2 * m.B1 + C3 * m.C1;
			result.C2 = C1 * m.A2 + C2 * m.B2 + C3 * m.C2;
			result.C3 = C1 * m.A3 + C2 * m.B3 + C3 * m.C3;

			return result;
		}

		Vector3 Mul(const Vector3& v) const
		{
			Vector3 result;
			result.X = A1 * v.X + A2 * v.Y + A3 * v.Z;
			result.Y = B1 * v.X + B2 * v.Y + B3 * v.Z;
			result.Z = C1 * v.X + C2 * v.Y + C3 * v.Z;
			return result;
		}

		const float* GetFloatPointer() const { return &A1; }
	};

}