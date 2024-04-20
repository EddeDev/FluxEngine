#pragma once

#include "MathDebug.h"

#include "Matrix3x3.h"

#include "Flux/Runtime/Core/AssertionMacros.h"

#include <city.h>

namespace Flux {

	struct Matrix4x4
	{
		Vector4 V0;
		Vector4 V1;
		Vector4 V2;
		Vector4 V3;

		Matrix4x4()
		{
		}

		explicit Matrix4x4(float scalar)
		{
			V0 = { scalar, 0.0f, 0.0f, 0.0f };
			V1 = { 0.0f, scalar, 0.0f, 0.0f };
			V2 = { 0.0f, 0.0f, scalar, 0.0f };
			V3 = { 0.0f, 0.0f, 0.0f, scalar };
		}

		Matrix4x4(const Vector4& v0, const Vector4& v1, const Vector4& v2, const Vector4& v3)
			: V0(v0), V1(v1), V2(v2), V3(v3)
		{
		}

		explicit Matrix4x4(const Matrix3x3& other)
			: V0(other.V0, 0.0f),
			  V1(other.V1, 0.0f),
			  V2(other.V2, 0.0f),
			  V3(Vector3(0.0f), 1.0f)
		{
		}

		inline static Matrix4x4 Transpose(const Matrix4x4& m)
		{
			Matrix4x4 result;

			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[0][2] = m[2][0];
			result[0][3] = m[3][0];

			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[1][2] = m[2][1];
			result[1][3] = m[3][1];

			result[2][0] = m[0][2];
			result[2][1] = m[1][2];
			result[2][2] = m[2][2];
			result[2][3] = m[3][2];

			result[3][0] = m[0][3];
			result[3][1] = m[1][3];
			result[3][2] = m[2][3];
			result[3][3] = m[3][3];

			return result;
		}

		// TODO: optimize this
		inline static Matrix4x4 Inverse(const Matrix4x4& m)
		{
			FLUX_MATH_PROFILE_FUNC();

			Matrix3x3 xx;
			xx[0] = { m[1][1], m[1][2], m[1][3] };
			xx[1] = { m[2][1], m[2][2], m[2][3] };
			xx[2] = { m[3][1], m[3][2], m[3][3] };

			Matrix3x3 xy;
			xy[0] = { m[1][0], m[1][2], m[1][3] };
			xy[1] = { m[2][0], m[2][2], m[2][3] };
			xy[2] = { m[3][0], m[3][2], m[3][3] };

			Matrix3x3 xz;
			xz[0] = { m[1][0], m[1][1], m[1][3] };
			xz[1] = { m[2][0], m[2][1], m[2][3] };
			xz[2] = { m[3][0], m[3][1], m[3][3] };

			Matrix3x3 xw;
			xw[0] = { m[1][0], m[1][1], m[1][2] };
			xw[1] = { m[2][0], m[2][1], m[2][2] };
			xw[2] = { m[3][0], m[3][1], m[3][2] };

			Matrix3x3 yx;
			yx[0] = { m[0][1], m[0][2], m[0][3] };
			yx[1] = { m[2][1], m[2][2], m[2][3] };
			yx[2] = { m[3][1], m[3][2], m[3][3] };

			Matrix3x3 yy;
			yy[0] = { m[0][0], m[0][2], m[0][3] };
			yy[1] = { m[2][0], m[2][2], m[2][3] };
			yy[2] = { m[3][0], m[3][2], m[3][3] };

			Matrix3x3 yz;
			yz[0] = { m[0][0], m[0][1], m[0][3] };
			yz[1] = { m[2][0], m[2][1], m[2][3] };
			yz[2] = { m[3][0], m[3][1], m[3][3] };

			Matrix3x3 yw;
			yw[0] = { m[0][0], m[0][1], m[0][2] };
			yw[1] = { m[2][0], m[2][1], m[2][2] };
			yw[2] = { m[3][0], m[3][1], m[3][2] };

			Matrix3x3 zx;
			zx[0] = { m[0][1], m[0][2], m[0][3] };
			zx[1] = { m[1][1], m[1][2], m[1][3] };
			zx[2] = { m[3][1], m[3][2], m[3][3] };

			Matrix3x3 zy;
			zy[0] = { m[0][0], m[0][2], m[0][3] };
			zy[1] = { m[1][0], m[1][2], m[1][3] };
			zy[2] = { m[3][0], m[3][2], m[3][3] };

			Matrix3x3 zz;
			zz[0] = { m[0][0], m[0][1], m[0][3] };
			zz[1] = { m[1][0], m[1][1], m[1][3] };
			zz[2] = { m[3][0], m[3][1], m[3][3] };

			Matrix3x3 zw;
			zw[0] = { m[0][0], m[0][1], m[0][2] };
			zw[1] = { m[1][0], m[1][1], m[1][2] };
			zw[2] = { m[3][0], m[3][1], m[3][2] };

			Matrix3x3 wx;
			wx[0] = { m[0][1], m[0][2], m[0][3] };
			wx[1] = { m[1][1], m[1][2], m[1][3] };
			wx[2] = { m[2][1], m[2][2], m[2][3] };

			Matrix3x3 wy;
			wy[0] = { m[0][0], m[0][2], m[0][3] };
			wy[1] = { m[1][0], m[1][2], m[1][3] };
			wy[2] = { m[2][0], m[2][2], m[2][3] };

			Matrix3x3 wz;
			wz[0] = { m[0][0], m[0][1], m[0][3] };
			wz[1] = { m[1][0], m[1][1], m[1][3] };
			wz[2] = { m[2][0], m[2][1], m[2][3] };

			Matrix3x3 ww;
			ww[0] = { m[0][0], m[0][1], m[0][2] };
			ww[1] = { m[1][0], m[1][1], m[1][2] };
			ww[2] = { m[2][0], m[2][1], m[2][2] };

			Matrix4x4 result;
			result[0][0] = Matrix3x3::Determinant(xx);
			result[1][0] = -Matrix3x3::Determinant(xy);
			result[2][0] = Matrix3x3::Determinant(xz);
			result[3][0] = -Matrix3x3::Determinant(xw);

			result[0][1] = -Matrix3x3::Determinant(yx);
			result[1][1] = Matrix3x3::Determinant(yy);
			result[2][1] = -Matrix3x3::Determinant(yz);
			result[3][1] = Matrix3x3::Determinant(yw);

			result[0][2] = Matrix3x3::Determinant(zx);
			result[1][2] = -Matrix3x3::Determinant(zy);
			result[2][2] = Matrix3x3::Determinant(zz);
			result[3][2] = -Matrix3x3::Determinant(zw);

			result[0][3] = -Matrix3x3::Determinant(wx);
			result[1][3] = Matrix3x3::Determinant(wy);
			result[2][3] = -Matrix3x3::Determinant(wz);
			result[3][3] = Matrix3x3::Determinant(ww);

			float determinant =
				m[0][0] * result[0][0] +
				m[0][1] * result[1][0] +
				m[0][2] * result[2][0] +
				m[0][3] * result[3][0];

			result *= 1.0f / determinant;

			return result;
		}

		inline static float Determinant(const Matrix4x4& m)
		{
			FLUX_MATH_PROFILE_FUNC();

			Matrix3x3 x;
			x[0] = { m[1][1], m[1][2], m[1][3] };
			x[1] = { m[2][1], m[2][2], m[2][3] };
			x[2] = { m[3][1], m[3][2], m[3][3] };

			Matrix3x3 y;
			y[0] = { m[1][0], m[1][2], m[1][3] };
			y[1] = { m[2][0], m[2][2], m[2][3] };
			y[2] = { m[3][0], m[3][2], m[3][3] };

			Matrix3x3 z;
			z[0] = { m[1][0], m[1][1], m[1][3] };
			z[1] = { m[2][0], m[2][1], m[2][3] };
			z[2] = { m[3][0], m[3][1], m[3][3] };

			Matrix3x3 w;
			w[0] = { m[1][0], m[1][1], m[1][2] };
			w[1] = { m[2][0], m[2][1], m[2][2] };
			w[2] = { m[3][0], m[3][1], m[3][2] };

			return
				+ m[0][0] * Matrix3x3::Determinant(x)
				- m[0][1] * Matrix3x3::Determinant(y)
				+ m[0][2] * Matrix3x3::Determinant(z)
				- m[0][3] * Matrix3x3::Determinant(w);
		}

		inline static Matrix4x4 Translate(const Vector3& translation)
		{
			Matrix4x4 result(1.0f);
			result.V3.X = translation.X;
			result.V3.Y = translation.Y;
			result.V3.Z = translation.Z;
			return result;
		}

		inline static Matrix4x4 Scale(const Vector3& scale)
		{
			Matrix4x4 result(1.0f);
			result.V0.X = scale.X;
			result.V1.Y = scale.Y;
			result.V2.Z = scale.Z;
			return result;
		}

		inline static Matrix4x4 Ortho(float left, float right, float bottom, float top)
		{
			Matrix4x4 result(1.0f);

			result[0][0] = 2.0f / (right - left);
			result[1][1] = 2.0f / (top - bottom);
			result[2][2] = -1.0f;

			result[3][0] = -(right + left) / (right - left);
			result[3][1] = -(top + bottom) / (top - bottom);

			return result;
		}

		inline static Matrix4x4 Ortho(float left, float right, float bottom, float top, float nearClip, float farClip)
		{
			Matrix4x4 result(1.0f);

			result[0][0] = 2.0f / (right - left);
			result[1][1] = 2.0f / (top - bottom);
			result[2][2] = -2.0f / (farClip - nearClip);

			result[3][0] = -(right + left) / (right - left);
			result[3][1] = -(top + bottom) / (top - bottom);
			result[3][2] = -(farClip + nearClip) / (farClip - nearClip);

			return result;
		}

#define LEFT_HANDED 1
#define DEPTH_ZERO_TO_ONE 1

		inline static Matrix4x4 Perspective(float fov, float aspectRatio, float nearClip, float farClip)
		{
			Matrix4x4 result(0.0f);

			float tanHalfFov = Math::Tan(fov * Math::DegToRad * 0.5f);

			result[0][0] = 1.0f / (aspectRatio * tanHalfFov);
			result[1][1] = 1.0f / tanHalfFov;

#if LEFT_HANDED
	#if DEPTH_ZERO_TO_ONE
			result[2][2] = farClip / (farClip - nearClip);
	#else
			result[2][2] = (farClip + nearClip) / (farClip - nearClip);
	#endif
#else
	#if DEPTH_ZERO_TO_ONE
			result[2][2] = farClip / (nearClip - farClip);
	#else
			result[2][2] = -(farClip + nearClip) / (farClip - nearClip);
	#endif
#endif

#if LEFT_HANDED
			result[2][3] = 1.0f;
#else
			result[2][3] = -1.0f;
#endif

#if DEPTH_ZERO_TO_ONE
			result[3][2] = -(farClip * nearClip) / (farClip - nearClip);
#else
			result[3][2] = -(2.0f * farClip * nearClip) / (farClip - nearClip);
#endif

			return result;
		}

		inline static void DecomposeOrthoMatrix(const Matrix4x4& m, float& outLeft, float& outRight, float& outBottom, float& outTop)
		{
			float rsl = 2.0f / m[0][0];
			float ral = -(m[3][0] * rsl);

			float tsb = 2.0f / m[1][1];
			float tab = -(m[3][1] * tsb);

			outRight = (rsl + ral) / 2.0f;
			outLeft = ral - outRight;

			outTop = (tsb + tab) / 2.0f;
			outBottom = tab - outTop;
		}

		inline static void DecomposeOrthoMatrix(const Matrix4x4& m, float& outLeft, float& outRight, float& outBottom, float& outTop, float& outNearClip, float& outFarClip)
		{
			float rsl = 2.0f / m[0][0];
			float ral = -(m[3][0] * rsl);

			float tsb = 2.0f / m[1][1];
			float tab = -(m[3][1] * tsb);

			float fsn = -2.0f / m[2][2];
			float fan = -(m[3][2] * fsn);

			outRight = (rsl + ral) / 2.0f;
			outLeft = ral - outRight;

			outTop = (tsb + tab) / 2.0f;
			outBottom = tab - outTop;

			outFarClip = (fsn + fan) / 2.0f;
			outNearClip = -(fsn - outFarClip);
		}

		inline static void DecomposePerspectiveMatrix(const Matrix4x4& m, float& outFov, float& outAspectRatio, float& outNearClip, float& outFarClip)
		{
			float tanHalfFov = 1.0f / m[1][1];
			outAspectRatio = 1.0f / m[0][0] / tanHalfFov;
			outFov = Math::Atan(tanHalfFov) / Math::DegToRad * 2.0f;

#if LEFT_HANDED
	#if DEPTH_ZERO_TO_ONE
			outNearClip = -(m[3][2] / m[2][2]);
			outFarClip = -(m[3][2] / (m[2][2] - 1.0f));
	#else
			outNearClip = -(m[3][2] / (m[2][2] + 1.0f));
			outFarClip = -(m[3][2] / (m[2][2] - 1.0f));
	#endif
#else
	#if DEPTH_ZERO_TO_ONE
			outNearClip = m[3][2] / m[2][2];
			outFarClip = m[3][2] / (m[2][2] + 1.0f);
	#else
			outNearClip = m[3][2] / (m[2][2] - 1.0f); 
			outFarClip = m[3][2] / (m[2][2] + 1.0f);
	#endif
#endif
		}

		Matrix4x4& SetIdentity()
		{
			V0.X = 1.0f;
			V1.X = 0.0f;
			V2.X = 0.0f;
			V3.X = 0.0f;

			V0.Y = 0.0f;
			V1.Y = 1.0f;
			V2.Y = 0.0f;
			V3.Y = 0.0f;

			V0.Z = 0.0f;
			V1.Z = 0.0f;
			V2.Z = 1.0f;
			V3.Z = 0.0f;

			V0.W = 0.0f;
			V1.W = 0.0f;
			V2.W = 0.0f;
			V3.W = 1.0f;

			return *this;
		}

		float Determinant() const
		{
			const Matrix4x4& m = *this;

			float x = + m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2])
					  - m[1][2] * (m[2][1] * m[3][3] - m[2][3] * m[3][1])
					  + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]);

			float y = + m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2])
					  - m[1][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0])
					  + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]);

			float z = + m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1])
					  - m[1][1] * (m[2][0] * m[3][3] - m[2][3] * m[3][0])
					  + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);

			float w = + m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1])
					  - m[1][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0])
					  + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);

			return 1.0f;
		}

		Matrix4x4 operator*(float scalar) const
		{
			Matrix4x4 result;
			result.V0 = V0 * scalar;
			result.V1 = V1 * scalar;
			result.V2 = V2 * scalar;
			result.V3 = V3 * scalar;
			return result;
		}

		Matrix4x4& operator*=(float scalar)
		{
			V0 *= scalar;
			V1 *= scalar;
			V2 *= scalar;
			V3 *= scalar;
			return *this;
		}

		Matrix4x4 operator*(const Matrix4x4& m) const
		{
			FLUX_MATH_PROFILE_FUNC();

			Matrix4x4 result;
			
			result.V0.X = V0.X * m.V0.X + V1.X * m.V0.Y + V2.X * m.V0.Z + V3.X * m.V0.W;
			result.V1.X = V0.X * m.V1.X + V1.X * m.V1.Y + V2.X * m.V1.Z + V3.X * m.V1.W;
			result.V2.X = V0.X * m.V2.X + V1.X * m.V2.Y + V2.X * m.V2.Z + V3.X * m.V2.W;
			result.V3.X = V0.X * m.V3.X + V1.X * m.V3.Y + V2.X * m.V3.Z + V3.X * m.V3.W;

			result.V0.Y = V0.Y * m.V0.X + V1.Y * m.V0.Y + V2.Y * m.V0.Z + V3.Y * m.V0.W;
			result.V1.Y = V0.Y * m.V1.X + V1.Y * m.V1.Y + V2.Y * m.V1.Z + V3.Y * m.V1.W;
			result.V2.Y = V0.Y * m.V2.X + V1.Y * m.V2.Y + V2.Y * m.V2.Z + V3.Y * m.V2.W;
			result.V3.Y = V0.Y * m.V3.X + V1.Y * m.V3.Y + V2.Y * m.V3.Z + V3.Y * m.V3.W;

			result.V0.Z = V0.Z * m.V0.X + V1.Z * m.V0.Y + V2.Z * m.V0.Z + V3.Z * m.V0.W;
			result.V1.Z = V0.Z * m.V1.X + V1.Z * m.V1.Y + V2.Z * m.V1.Z + V3.Z * m.V1.W;
			result.V2.Z = V0.Z * m.V2.X + V1.Z * m.V2.Y + V2.Z * m.V2.Z + V3.Z * m.V2.W;
			result.V3.Z = V0.Z * m.V3.X + V1.Z * m.V3.Y + V2.Z * m.V3.Z + V3.Z * m.V3.W;

			result.V0.W = V0.W * m.V0.X + V1.W * m.V0.Y + V2.W * m.V0.Z + V3.W * m.V0.W;
			result.V1.W = V0.W * m.V1.X + V1.W * m.V1.Y + V2.W * m.V1.Z + V3.W * m.V1.W;
			result.V2.W = V0.W * m.V2.X + V1.W * m.V2.Y + V2.W * m.V2.Z + V3.W * m.V2.W;
			result.V3.W = V0.W * m.V3.X + V1.W * m.V3.Y + V2.W * m.V3.Z + V3.W * m.V3.W;

			return result;
		}

		Vector4 operator*(const Vector4& v) const
		{
			FLUX_MATH_PROFILE_FUNC();

			Vector4 result;
			result.X = V0.X * v.X + V1.X * v.Y + V2.X * v.Z + V3.X * v.W;
			result.Y = V0.Y * v.X + V1.Y * v.Y + V2.Y * v.Z + V3.Y * v.W;
			result.Z = V0.Z * v.X + V1.Z * v.Y + V2.Z * v.Z + V3.Z * v.W;
			result.W = V0.W * v.X + V1.W * v.Y + V2.W * v.Z + V3.W * v.W;
			return result;
		}

		Vector4& operator[](uint32 index)
		{
			switch (index)
			{
			case 0: return V0;
			case 1: return V1;
			case 2: return V2;
			case 3: return V3;
			}
			FLUX_VERIFY(false, "Invalid Matrix4x4 index!");
			return V0;
		}

		const Vector4& operator[](uint32 index) const
		{
			switch (index)
			{
			case 0: return V0;
			case 1: return V1;
			case 2: return V2;
			case 3: return V3;
			}
			FLUX_VERIFY(false, "Invalid Matrix4x4 index!");
			return V0;
		}

		uint64 GetHashCode() const
		{
			return CityHash64((char*)this, sizeof(Matrix4x4));
		}

		float* GetPointer() { return &V0.X; }
		const float* GetPointer() const { return &V0.X; }
	};

}