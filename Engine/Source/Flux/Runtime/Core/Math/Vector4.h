#pragma once

namespace Flux {

	struct Vector4
	{
		float X, Y, Z, W;

		Vector4()
			: X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
		{
		}

		Vector4(float x, float y, float z, float w)
			: X(x), Y(y), Z(z), W(w)
		{
		}

		Vector4 operator+(const Vector4& v)
		{
			Vector4 result;
			result.X = X + v.X;
			result.Y = Y + v.Y;
			result.Z = Z + v.Z;
			result.W = W + v.W;
			return result;
		}

		Vector4 operator-(const Vector4& v)
		{
			Vector4 result;
			result.X = X - v.X;
			result.Y = Y - v.Y;
			result.Z = Z - v.Z;
			result.W = W - v.W;
			return result;
		}

		Vector4 operator*(const Vector4& v)
		{
			Vector4 result;
			result.X = X * v.X;
			result.Y = Y * v.Y;
			result.Z = Z * v.Z;
			result.W = W * v.W;
			return result;
		}

		Vector4 operator*(float scalar)
		{
			Vector4 result;
			result.X = X * scalar;
			result.Y = Y * scalar;
			result.Z = Z * scalar;
			result.W = W * scalar;
			return result;
		}

		Vector4 operator/(const Vector4& v)
		{
			Vector4 result;
			result.X = X / v.X;
			result.Y = Y / v.Y;
			result.Z = Z / v.Z;
			result.W = W / v.W;
			return result;
		}

		Vector4 operator/(float scalar)
		{
			Vector4 result;
			result.X = X / scalar;
			result.Y = Y / scalar;
			result.Z = Z / scalar;
			result.W = W / scalar;
			return result;
		}

		const float* GetFloatPointer() const { return &X; }
	};

}