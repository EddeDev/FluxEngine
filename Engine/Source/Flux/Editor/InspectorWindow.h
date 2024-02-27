#pragma once

#include "EditorWindow.h"

namespace Flux {

	class InspectorWindow : public EditorWindow
	{
	public:
		InspectorWindow();
		virtual ~InspectorWindow();

		virtual void OnImGuiRender() override;
	private:
		bool DrawVector3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		bool DrawVector4Control(const std::string& label, Vector4& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	};

}