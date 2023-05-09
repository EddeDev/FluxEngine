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
			FLUX_ASSERT(s_Instance, "Engine instance is nullptr!");
			return (TEngine&)*s_Instance;
		}
	protected:
		virtual void OnInit() {}
		virtual void OnExit() {}
		virtual void OnUpdate() {}
 	protected:
		inline static Engine* s_Instance = nullptr;

		Shared<Window> m_Window;
		bool m_Running = true;
		
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		float m_LastTime = 0.0f;
		uint32 m_FrameCounter = 0;
		uint32 m_FramesPerSecond = 0;
	};

}