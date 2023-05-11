#pragma once

#include "Flux/Runtime/Engine/Engine.h"

namespace Flux {

	class EditorEngine : public Engine
	{
	public:
		EditorEngine() {}

		virtual void OnInit() override;
		virtual void OnExit() override;
		virtual void OnUpdate() override;
	private:
		void OnMenuCallback(WindowMenu menu, uint32 menuID);
	private:
		Unique<Window> m_AboutWindow;
	};

}