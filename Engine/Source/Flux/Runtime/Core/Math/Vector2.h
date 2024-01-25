#pragma once

namespace Flux {

	struct Vector2
	{
		float X, Y;

		Vector2()
			: X(0.0f), Y(0.0f)
		{
		}

		explicit Vector2(float scalar)
			: X(scalar), Y(scalar)
		{
		}

		Vector2(float x, float y)
			: X(x), Y(y)
		{
		}

		Vector2 operator+(const Vector2& v)
		{
			Vector2 result;
			result.X = X + v.X;
			result.Y = Y + v.Y;
			return result;
		}

		Vector2 operator-(const Vector2& v)
		{
			Vector2 result;
			result.X = X - v.X;
			result.Y = Y - v.Y;
			return result;
		}

		Vector2 operator*(const Vector2& v)
		{
			Vector2 result;
			result.X = X * v.X;
			result.Y = Y * v.Y;
			return result;
		}

		Vector2 operator*(float scalar)
		{
			Vector2 result;
			result.X = X * scalar;
			result.Y = Y * scalar;
			return result;
		}

		Vector2 operator/(const Vector2& v)
		{
			Vector2 result;
			result.X = X / v.X;
			result.Y = Y / v.Y;
			return result;
		}

		Vector2 operator/(float scalar)
		{
			Vector2 result;
			result.X = X / scalar;
			result.Y = Y / scalar;
			return result;
		}

		const float* GetFloatPointer() const { return &X; }
	};

}