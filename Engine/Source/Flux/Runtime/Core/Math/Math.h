#pragma once

namespace Flux {

	namespace Math {

		inline constexpr float PI = 3.1415926f;
		inline constexpr float DegToRad = PI / 180.0f;
		inline constexpr float RadToDeg = 1.0f / DegToRad;

		template<typename T>
		inline T Sin(T value)
		{
			return sin(value);
		}

		template<typename T>
		inline T Cos(T value)
		{
			return cos(value);
		}

		template<typename T>
		inline T Tan(T value)
		{
			return tan(value);
		}

		template<typename T>
		inline T Asin(T value)
		{
			return asin(value);
		}

		template<typename T>
		inline T Acos(T value)
		{
			return acos(value);
		}

		template<typename T>
		inline T Atan(T x)
		{
			return atan(x);
		}

		template<typename T>
		inline T Atan(T y, T x)
		{
			return atan2(y, x);
		}

		template<typename T>
		inline T Atan2(T y, T x)
		{
			return atan2(y, x);
		}

		template<typename T>
		inline T Sqrt(T value)
		{
			return sqrt(value);
		}

		template<typename T>
		inline T Abs(T value)
		{
			return abs(value);
		}

		template<typename T>
		inline T Log(T value)
		{
			return log(value);
		}

		template<typename T>
		inline T Log10(T value)
		{
			return log10(value);
		}

		template<typename T>
		inline T Ceil(T value)
		{
			return ceil(value);
		}

		template<typename T>
		inline T Floor(T value)
		{
			return floor(value);
		}

		template<typename T>
		inline T Round(T value)
		{
			return round(value);
		}

		template<typename T>
		inline T Min(T a, T b)
		{
			return a < b ? a : b;
		}

		template<typename T>
		inline T Max(T a, T b)
		{
			return a > b ? a : b;
		}

		template<typename T>
		inline T Clamp(T value, T min, T max)
		{
			if (value < min)
				value = min;
			else if (value > max)
				value = max;
			return value;
		}

	}

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
	};

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
	};

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
				return 2.0f * Math::Atan(X, W);

			return Math::Atan(y, x);
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

			return Math::Atan(y, x);
		}

		Vector3 GetEulerAngles() const
		{
			return { GetPitch(), GetYaw(), GetRoll() };
		}
	};

	struct Matrix
	{
		float A1, B1, C1, D1;
		float A2, B2, C2, D2;
		float A3, B3, C3, D3;
		float A4, B4, C4, D4;

		Matrix()
			: A1(0.0f), A2(0.0f), A3(0.0f), A4(0.0f),
			  B1(0.0f), B2(0.0f), B3(0.0f), B4(0.0f),
			  C1(0.0f), C2(0.0f), C3(0.0f), C4(0.0f),
			  D1(0.0f), D2(0.0f), D3(0.0f), D4(0.0f)
		{
		}

		explicit Matrix(float scalar)
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

		void SetIdentity()
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
		}

		void Translate(const Vector3& translation)
		{
			A4 += translation.X;
			B4 += translation.Y;
			C4 += translation.Z;
		}

		void Scale(const Vector3& scale)
		{
			A1 *= scale.X;
			B2 *= scale.Y;
			C3 *= scale.Z;
		}

		void SetEulerAnglesX(float x)
		{
			B2 = Math::Cos(x);
			C2 = -Math::Sin(x);
			B3 = Math::Sin(x);
			C3 = Math::Cos(x);
		}

		void SetEulerAnglesY(float y)
		{
			A1 = Math::Cos(y);
			C1 = Math::Sin(y);
			A3 = -Math::Sin(y);
			C3 = Math::Cos(y);
		}

		void SetEulerAnglesZ(float z)
		{
			A1 = Math::Cos(z);
			B1 = -Math::Sin(z);
			A2 = Math::Sin(z);
			B2 = Math::Cos(z);
		}

		void SetEulerAngles(const Vector3& eulerAngles)
		{
			SetEulerAnglesX(eulerAngles.X);
			SetEulerAnglesY(eulerAngles.Y);
			SetEulerAnglesZ(eulerAngles.Z);
		}

		Matrix Mul(const Matrix& m) const
		{
			Matrix result;

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

		Vector4 Mul(const Vector4& v) const
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