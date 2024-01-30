#pragma once

#include "MathUtils.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "Quaternion.h"

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"

namespace Flux {

	namespace Math {

		static Matrix4x4 BuildTransformationMatrix(const Vector3& position, const Quaternion& rotation, const Vector3& scale = Vector3(1.0f))
		{
			Matrix4x4 positionMatrix = Matrix4x4::Translate(position);
			Matrix4x4 rotationMatrix = rotation.ToMatrix4x4();
			Matrix4x4 scaleMatrix = Matrix4x4::Scale(scale);

			return positionMatrix * rotationMatrix * scaleMatrix;
		}

		static Matrix4x4 BuildTransformationMatrix(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale = Vector3(1.0f))
		{
			return BuildTransformationMatrix(position, Quaternion(eulerAngles), scale);
		}

	}

}