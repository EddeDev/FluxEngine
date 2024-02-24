#pragma once

#include "EditorWindow.h"

namespace Flux {

	class InspectorWindow : public EditorWindow
	{
	public:
		InspectorWindow();
		virtual ~InspectorWindow();

		virtual void OnImGuiRender() override;
	};

}