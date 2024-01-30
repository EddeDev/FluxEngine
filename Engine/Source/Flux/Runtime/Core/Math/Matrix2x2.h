#pragma once

#include "Vector2.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	struct Matrix2x2
	{
		Vector2 V0;
		Vector2 V1;

		Matrix2x2()
			: V0(Vector2(0.0f)),
			  V1(Vector2(0.0f))
		{
		}

		explicit Matrix2x2(float scalar)
		{
			V0 = { scalar, 0.0f };
			V1 = { 0.0f, scalar };
		}

		Matrix2x2(const Vector2& v0, const Vector2& v1)
			: V0(v0), V1(v1)
		{
		}

		static Matrix2x2 Transpose(const Matrix2x2& m)
		{
			Matrix2x2 result;
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			return result;
		}

		static Matrix2x2 Inverse(const Matrix2x2& m)
		{
			float oneOverDeterminant = 1.0f / Determinant(m);

			Matrix2x2 result;
			result[0][0] =  m[1][1] * oneOverDeterminant;
			result[0][1] = -m[0][1] * oneOverDeterminant;
			result[1][0] = -m[1][0] * oneOverDeterminant;
			result[1][1] =  m[0][0] * oneOverDeterminant;
			return result;
		}
		
		static float Determinant(const Matrix2x2& m)
		{
			return m[0][0] * m[1][1] - m[0][1] * m[1][0];
		}

		Matrix2x2& SetIdentity()
		{
			V0 = { 1.0f, 0.0f };
			V1 = { 0.0f, 1.0f };
			return *this;
		}

		Matrix2x2 operator*(float scalar) const
		{
			Matrix2x2 result;
			result.V0 = V0 * scalar;
			result.V1 = V1 * scalar;
			return result;
		}

		Matrix2x2& operator*=(float scalar)
		{
			V0 *= scalar;
			V1 *= scalar;
			return *this;
		}

		Matrix2x2 operator*(const Matrix2x2& m) const
		{
			Matrix2x2 result;

			result.V0.X = V0.X * m.V0.X + V1.X * m.V0.Y;
			result.V1.X = V0.X * m.V1.X + V1.X * m.V1.Y;

			result.V0.Y = V0.Y * m.V0.X + V1.Y * m.V0.Y;
			result.V1.Y = V0.Y * m.V1.X + V1.Y * m.V1.Y;

			return result;
		}

		Vector2 operator*(const Vector2& v) const
		{
			Vector2 result;
			result.X = V0.X * v.X + V1.X * v.Y;
			result.Y = V0.Y * v.X + V1.Y * v.Y;
			return result;
		}

		Vector2& operator[](uint32 index)
		{
			switch (index)
			{
			case 0: return V0;
			case 1: return V1;
			}
			FLUX_VERIFY(false, "Invalid Matrix2x2 index!");
			return V0;
		}

		const Vector2& operator[](uint32 index) const
		{
			switch (index)
			{
			case 0: return V0;
			case 1: return V1;
			}
			FLUX_VERIFY(false, "Invalid Matrix2x2 index!");
			return V0;
		}

		const float* GetFloatPointer() const { return &V0.X; }
	};

}