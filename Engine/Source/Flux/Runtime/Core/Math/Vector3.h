#pragma once

namespace Flux {

	struct Vector3
	{
		float X, Y, Z;

		Vector3()
			: X(0.0f), Y(0.0f), Z(0.0f)
		{
		}

		explicit Vector3(float scalar)
			: X(scalar), Y(scalar), Z(scalar)
		{
		}

		Vector3(float x, float y, float z)
			: X(x), Y(y), Z(z)
		{
		}

		float LengthSquared() const
		{
			return X * X + Y * Y + Z * Z;
		}

		float Length() const
		{
			return Math::Sqrt(LengthSquared());
		}

		Vector3& Normalize()
		{
			float invLength = 1.0f / Length();
			X *= invLength;
			Y *= invLength;
			Z *= invLength;
			return *this;
		}

		Vector3 operator-() const
		{
			return { -X, -Y, -Z };
		}

		Vector3 operator+(const Vector3& v)
		{
			Vector3 result;
			result.X = X + v.X;
			result.Y = Y + v.Y;
			result.Z = Z + v.Z;
			return result;
		}

		Vector3 operator-(const Vector3& v)
		{
			Vector3 result;
			result.X = X - v.X;
			result.Y = Y - v.Y;
			result.Z = Z - v.Z;
			return result;
		}

		Vector3 operator*(const Vector3& v)
		{
			Vector3 result;
			result.X = X * v.X;
			result.Y = Y * v.Y;
			result.Z = Z * v.Z;
			return result;
		}

		Vector3 operator*(float scalar)
		{
			Vector3 result;
			result.X = X * scalar;
			result.Y = Y * scalar;
			result.Z = Z * scalar;
			return result;
		}

		Vector3 operator/(const Vector3& v)
		{
			Vector3 result;
			result.X = X / v.X;
			result.Y = Y / v.Y;
			result.Z = Z / v.Z;
			return result;
		}

		Vector3 operator/(float scalar)
		{
			Vector3 result;
			result.X = X / scalar;
			result.Y = Y / scalar;
			result.Z = Z / scalar;
			return result;
		}

		const float* GetFloatPointer() const { return &X; }
	};

}