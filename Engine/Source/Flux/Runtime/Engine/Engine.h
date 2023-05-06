#pragma once

#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	class Engine
	{
	protected:
		Engine();
	public:
		~Engine();

		void Run();
		void Close(bool restart = false);

		template<typename TEngine = Engine>
		static TEngine& Get()
		{
			return (TEngine&)*s_Instance;
		}
	protected:
		virtual void OnInit() {}
		virtual void OnExit() {}
		virtual void OnUpdate() {}
 	private:
		inline static Engine* s_Instance = nullptr;

		Shared<Window> m_Window;
		bool m_Running = true;
	};

}