#pragma once

#include "Vector3.h"
#include "Matrix2x2.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Matrix3x3
	{
		Vector3 V0;
		Vector3 V1;
		Vector3 V2;

		Matrix3x3()
			: V0(Vector3(0.0f)),
			  V1(Vector3(0.0f)),
			  V2(Vector3(0.0f))
		{
		}

		explicit Matrix3x3(float scalar)
		{
			V0 = { scalar, 0.0f, 0.0f };
			V1 = { 0.0f, scalar, 0.0f };
			V2 = { 0.0f, 0.0f, scalar };
		}

		static Matrix3x3 Transpose(const Matrix3x3& m)
		{
			Matrix3x3 result;
			
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[0][2] = m[2][0];

			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[1][2] = m[2][1];

			result[2][0] = m[0][2];
			result[2][1] = m[1][2];
			result[2][2] = m[2][2];

			return result;
		}

		static Matrix3x3 Inverse(const Matrix3x3& m)
		{
			float oneOverDeterminant = 1.0f / Determinant(m);

			Matrix3x3 result;
			result[0][0] =  ((m[1][1] * m[2][2]) - (m[1][2] * m[2][1])) * oneOverDeterminant;
			result[1][0] = -((m[1][0] * m[2][2]) - (m[1][2] * m[2][0])) * oneOverDeterminant;
			result[2][0] =  ((m[1][0] * m[2][1]) - (m[1][1] * m[2][0])) * oneOverDeterminant;
			result[0][1] = -((m[0][1] * m[2][2]) - (m[0][2] * m[2][1])) * oneOverDeterminant;
			result[1][1] =  ((m[0][0] * m[2][2]) - (m[0][2] * m[2][0])) * oneOverDeterminant;
			result[2][1] = -((m[0][0] * m[2][1]) - (m[0][1] * m[2][0])) * oneOverDeterminant;
			result[0][2] =  ((m[0][1] * m[1][2]) - (m[0][2] * m[1][1])) * oneOverDeterminant;
			result[1][2] = -((m[0][0] * m[1][2]) - (m[0][2] * m[1][0])) * oneOverDeterminant;
			result[2][2] =  ((m[0][0] * m[1][1]) - (m[0][1] * m[1][0])) * oneOverDeterminant;
			return result;
		}

		static float Determinant(const Matrix3x3& m)
		{
			Matrix2x2 x;
			x[0] = { m[1][1], m[1][2] };
			x[1] = { m[2][1], m[2][2] };

			Matrix2x2 y;
			y[0] = { m[1][0], m[1][2] };
			y[1] = { m[2][0], m[2][2] };

			Matrix2x2 z;
			z[0] = { m[1][0], m[1][1] };
			z[1] = { m[2][0], m[2][1] };

			return + m[0][0] * Matrix2x2::Determinant(x)
				   - m[0][1] * Matrix2x2::Determinant(y)
				   + m[0][2] * Matrix2x2::Determinant(z);
		}

		Matrix3x3& SetIdentity()
		{
			V0 = { 1.0f, 0.0f, 0.0f };
			V1 = { 0.0f, 1.0f, 0.0f };
			V2 = { 0.0f, 0.0f, 1.0f };
			return *this;
		}

		Matrix3x3 operator*(float scalar) const
		{
			Matrix3x3 result;
			result.V0 = V0 * scalar;
			result.V1 = V1 * scalar;
			result.V2 = V2 * scalar;
			return result;
		}

		Matrix3x3& operator*=(float scalar)
		{
			V0 *= scalar;
			V1 *= scalar;
			V2 *= scalar;
			return *this;
		}

		Matrix3x3 operator*(const Matrix3x3& m) const
		{
			Matrix3x3 result;

			result.V0.X = V0.X * m.V0.X + V1.X * m.V0.Y + V2.X * m.V0.Z;
			result.V1.X = V0.X * m.V1.X + V1.X * m.V1.Y + V2.X * m.V1.Z;
			result.V2.X = V0.X * m.V2.X + V1.X * m.V2.Y + V2.X * m.V2.Z;

			result.V0.Y = V0.Y * m.V0.X + V1.Y * m.V0.Y + V2.Y * m.V0.Z;
			result.V1.Y = V0.Y * m.V1.X + V1.Y * m.V1.Y + V2.Y * m.V1.Z;
			result.V2.Y = V0.Y * m.V2.X + V1.Y * m.V2.Y + V2.Y * m.V2.Z;

			result.V0.Z = V0.Z * m.V0.X + V1.Z * m.V0.Y + V2.Z * m.V0.Z;
			result.V1.Z = V0.Z * m.V1.X + V1.Z * m.V1.Y + V2.Z * m.V1.Z;
			result.V2.Z = V0.Z * m.V2.X + V1.Z * m.V2.Y + V2.Z * m.V2.Z;

			return result;
		}

		Vector3 operator*(const Vector3& v) const
		{
			Vector3 result;
			result.X = V0.X * v.X + V1.X * v.Y + V2.X * v.Z;
			result.Y = V0.Y * v.X + V1.Y * v.Y + V2.Y * v.Z;
			result.Z = V0.Z * v.X + V1.Z * v.Y + V2.Z * v.Z;
			return result;
		}

		Vector3& operator[](uint32 index)
		{
			switch (index)
			{
			case 0: return V0;
			case 1: return V1;
			case 2: return V2;
			}
			FLUX_VERIFY(false, "Invalid Matrix3x3 index!");
			return V0;
		}

		const Vector3& operator[](uint32 index) const
		{
			switch (index)
			{
			case 0: return V0;
			case 1: return V1;
			case 2: return V2;
			}
			FLUX_VERIFY(false, "Invalid Matrix3x3 index!");
			return V0;
		}

		const float* GetFloatPointer() const { return &V0.X; }
	};

}