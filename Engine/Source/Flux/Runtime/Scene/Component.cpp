#include "FluxPCH.h"
#include "Component.h"

namespace Flux {

	Transform::Transform(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale)
		: m_Position(position), m_Rotation(eulerAngles), m_Scale(scale)
	{
		OnChanged();
	}

	void Transform::OnImGuiRender()
	{

	}

}