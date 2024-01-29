#pragma once

#include "Vector3.h"

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

		Matrix3x3& SetIdentity()
		{
			V0 = { 1.0f, 0.0f, 0.0f };
			V1 = { 0.0f, 1.0f, 0.0f };
			V2 = { 0.0f, 0.0f, 1.0f };
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