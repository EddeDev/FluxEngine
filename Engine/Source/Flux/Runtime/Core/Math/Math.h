#pragma once

#include "MathUtils.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "Quaternion.h"

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

#include "IntRect.h"

namespace Flux {

	namespace Math {

		inline static Matrix4x4 BuildTransformationMatrix(const Vector3& position, const Quaternion& rotation, const Vector3& scale = Vector3(1.0f))
		{
			Matrix4x4 positionMatrix = Matrix4x4::Translate(position);
			Matrix4x4 rotationMatrix = rotation.ToMatrix4x4();
			Matrix4x4 scaleMatrix = Matrix4x4::Scale(scale);

			return positionMatrix * rotationMatrix * scaleMatrix;
		}

		inline static Matrix4x4 BuildTransformationMatrix(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale = Vector3(1.0f))
		{
			return BuildTransformationMatrix(position, Quaternion(eulerAngles * Math::DegToRad), scale);
		}

		inline static bool DecomposeTransformationMatrix(const Matrix4x4& m, Vector3& outPosition, Quaternion& outOrientation, Vector3& outScale)
		{
			// From https://github.com/g-truc/glm/blob/b101e8f3de31af8c06932e03a447fd1c67ff5fa4/glm/gtx/matrix_decompose.inl#L33

			Matrix4x4 localMatrix(m);
			if (Math::EpsilonEqual(localMatrix[3][3], 0.0f))
				return false;

			for (uint32 i = 0; i < 4; i++)
			{
				for (uint32 j = 0; j < 4; j++)
					localMatrix[i][j] /= localMatrix[3][3];
			}

			if (Math::EpsilonNotEqual(localMatrix[0][3], 0.0f) ||
				Math::EpsilonNotEqual(localMatrix[1][3], 0.0f) || 
				Math::EpsilonNotEqual(localMatrix[2][3], 0.0f))
			{
				localMatrix[0][3] = 0.0f;
				localMatrix[1][3] = 0.0f;
				localMatrix[2][3] = 0.0f;
				localMatrix[3][3] = 1.0f;
			}

			outPosition = {
				localMatrix[3][0],
				localMatrix[3][1],
				localMatrix[3][2]
			};

			localMatrix[3] = { 0.0f, 0.0f, 0.0f, localMatrix[3].W };

			Vector3 row[3];
			for (uint32 i = 0; i < 3; i++)
			{
				for (uint32 j = 0; j < 3; j++)
					row[i][j] = localMatrix[i][j];
			}

			outScale.X = row[0].Length();
			row[0] /= row[0].Length();

			outScale.Y = row[1].Length();
			row[1] /= row[1].Length();

			outScale.Z = row[2].Length();
			row[2] /= row[2].Length();

			Vector3 Pdum3 = Vector3::Cross(row[1], row[2]);
			if (Vector3::Dot(row[0], Pdum3) < 0.0f)
			{
				for (uint32 i = 0; i < 3; i++)
				{
					outScale[i] *= -1.0f;
					row[i] *= -1.0f;
				}
			}

			float trace = row[0].X + row[1].Y + row[2].Z;
			if (trace > 0.0f)
			{
				float root = Math::Sqrt(trace + 1.0f);
				outOrientation.W = root * 0.5f;
				root = 0.5f / root;
				outOrientation.X = root * (row[1].Z - row[2].Y);
				outOrientation.Y = root * (row[2].X - row[0].Z);
				outOrientation.Z = root * (row[0].Y - row[1].X);
			}
			else
			{
				static uint32 Next[3] = { 1, 2, 0 };

				uint32 i = 0;
				if (row[1].Y > row[0].X)
					i = 1;
				if (row[2].Z > row[i][i])
					i = 2;

				uint32 j = Next[i];
				uint32 k = Next[j];

				float root = Math::Sqrt(row[i][i] - row[j][j] - row[k][k] + 1.0f);

				outOrientation[i] = root * 0.5f;
				root = 0.5f / root;
				outOrientation[j] = root * (row[i][j] + row[j][i]);
				outOrientation[k] = root * (row[i][k] + row[k][i]);
				outOrientation.W = root * (row[j][k] - row[k][j]);
			}

			return true;
		}

	}

}