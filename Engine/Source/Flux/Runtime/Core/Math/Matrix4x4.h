#pragma once

#include "Matrix3x3.h"

namespace Flux {

	struct Matrix4x4
	{
		float A1, B1, C1, D1;
		float A2, B2, C2, D2;
		float A3, B3, C3, D3;
		float A4, B4, C4, D4;

		Matrix4x4()
			: A1(0.0f), A2(0.0f), A3(0.0f), A4(0.0f),
			B1(0.0f), B2(0.0f), B3(0.0f), B4(0.0f),
			C1(0.0f), C2(0.0f), C3(0.0f), C4(0.0f),
			D1(0.0f), D2(0.0f), D3(0.0f), D4(0.0f)
		{
		}

		explicit Matrix4x4(float scalar)
		{
			A1 = scalar;
			A2 = 0.0f;
			A3 = 0.0f;
			A4 = 0.0f;

			B1 = 0.0f;
			B2 = scalar;
			B3 = 0.0f;
			B4 = 0.0f;

			C1 = 0.0f;
			C2 = 0.0f;
			C3 = scalar;
			C4 = 0.0f;

			D1 = 0.0f;
			D2 = 0.0f;
			D3 = 0.0f;
			D4 = scalar;
		}

		explicit Matrix4x4(const Matrix3x3& other)
			: A1(other.A1), A2(other.A2), A3(other.A3), A4(0.0f),
			B1(other.B1), B2(other.B2), B3(other.B3), B4(0.0f),
			C1(other.C1), C2(other.C2), C3(other.C3), C4(0.0f),
			D1(0.0f), D2(0.0f), D3(0.0f), D4(1.0f)
		{
		}

		static Matrix4x4 Translate(const Vector3& translation)
		{
			Matrix4x4 result(1.0f);
			result.A4 = translation.X;
			result.B4 = translation.Y;
			result.C4 = translation.Z;
			return result;
		}

		static Matrix4x4 Scale(const Vector3& scale)
		{
			Matrix4x4 result(1.0f);
			result.A1 = scale.X;
			result.B2 = scale.Y;
			result.C3 = scale.Z;
			return result;
		}

		static Matrix4x4 Ortho(float left, float right, float bottom, float top)
		{
			Matrix4x4 result(1.0f);

			result.A1 = 2.0f / (right - left);
			result.B2 = 2.0f / (top - bottom);
			result.C3 = -1.0f;

			result.A4 = -(right + left) / (right - left);
			result.B4 = -(top + bottom) / (top - bottom);

			return result;
		}

		static Matrix4x4 Ortho(float left, float right, float bottom, float top, float nearClip, float farClip)
		{
			Matrix4x4 result(1.0f);

			result.A1 = 2.0f / (right - left);
			result.B2 = 2.0f / (top - bottom);
			result.C3 = -2.0f / (farClip - nearClip);

			result.A4 = -(right + left) / (right - left);
			result.B4 = -(top + bottom) / (top - bottom);
			result.C4 = -(farClip + nearClip) / (farClip - nearClip);

			return result;
		}

		static Matrix4x4 Perspective(float fov, float aspectRatio, float nearClip, float farClip)
		{
			Matrix4x4 result(0.0f);

#define LEFT_HANDED 1
#define DEPTH_ZERO_TO_ONE 1

			float tanHalfFov = Math::Tan(fov * Math::DegToRad * 0.5f);

			result.A1 = 1.0f / (aspectRatio * tanHalfFov);
			result.B2 = 1.0f / tanHalfFov;

#if LEFT_HANDED
	#if DEPTH_ZERO_TO_ONE
			result.C3 = farClip / (farClip - nearClip);
	#else
			result.C3 = (farClip + nearClip) / (farClip - nearClip);
	#endif
			result.D3 = 1.0f;
#else
			result.C3 = -(farClip + nearClip) / (farClip - nearClip);
			result.D3 = -1.0f;
#endif

#if DEPTH_ZERO_TO_ONE
			result.C4 = -(farClip * nearClip) / (farClip - nearClip);
#else
			result.C4 = -(2.0f * farClip * nearClip) / (farClip - nearClip);
#endif

			return result;
		}

		Matrix4x4& SetIdentity()
		{
			A1 = 1.0f;
			A2 = 0.0f;
			A3 = 0.0f;
			A4 = 0.0f;

			B1 = 0.0f;
			B2 = 1.0f;
			B3 = 0.0f;
			B4 = 0.0f;

			C1 = 0.0f;
			C2 = 0.0f;
			C3 = 1.0f;
			C4 = 0.0f;

			D1 = 0.0f;
			D2 = 0.0f;
			D3 = 0.0f;
			D4 = 1.0f;

			return *this;
		}

		Matrix4x4 operator*(const Matrix4x4& m) const
		{
			Matrix4x4 result;

			result.A1 = A1 * m.A1 + A2 * m.B1 + A3 * m.C1 + A4 * m.D1;
			result.A2 = A1 * m.A2 + A2 * m.B2 + A3 * m.C2 + A4 * m.D2;
			result.A3 = A1 * m.A3 + A2 * m.B3 + A3 * m.C3 + A4 * m.D3;
			result.A4 = A1 * m.A4 + A2 * m.B4 + A3 * m.C4 + A4 * m.D4;

			result.B1 = B1 * m.A1 + B2 * m.B1 + B3 * m.C1 + B4 * m.D1;
			result.B2 = B1 * m.A2 + B2 * m.B2 + B3 * m.C2 + B4 * m.D2;
			result.B3 = B1 * m.A3 + B2 * m.B3 + B3 * m.C3 + B4 * m.D3;
			result.B4 = B1 * m.A4 + B2 * m.B4 + B3 * m.C4 + B4 * m.D4;

			result.C1 = C1 * m.A1 + C2 * m.B1 + C3 * m.C1 + C4 * m.D1;
			result.C2 = C1 * m.A2 + C2 * m.B2 + C3 * m.C2 + C4 * m.D2;
			result.C3 = C1 * m.A3 + C2 * m.B3 + C3 * m.C3 + C4 * m.D3;
			result.C4 = C1 * m.A4 + C2 * m.B4 + C3 * m.C4 + C4 * m.D4;

			result.D1 = D1 * m.A1 + D2 * m.B1 + D3 * m.C1 + D4 * m.D1;
			result.D2 = D1 * m.A2 + D2 * m.B2 + D3 * m.C2 + D4 * m.D2;
			result.D3 = D1 * m.A3 + D2 * m.B3 + D3 * m.C3 + D4 * m.D3;
			result.D4 = D1 * m.A4 + D2 * m.B4 + D3 * m.C4 + D4 * m.D4;

			return result;
		}

		Vector4 operator*(const Vector4& v) const
		{
			Vector4 result;
			result.X = A1 * v.X + A2 * v.Y + A3 * v.Z + A4 * v.W;
			result.Y = B1 * v.X + B2 * v.Y + B3 * v.Z + B4 * v.W;
			result.Z = C1 * v.X + C2 * v.Y + C3 * v.Z + C4 * v.W;
			result.W = D1 * v.X + D2 * v.Y + D3 * v.Z + D4 * v.W;
			return result;
		}

		const float* GetFloatPointer() const { return &A1; }
	};

}