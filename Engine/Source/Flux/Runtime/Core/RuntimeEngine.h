#pragma once

#include "Engine.h"

namespace Flux {

	class RuntimeEngine : public Engine
	{
	public:
		RuntimeEngine(const EngineCreateInfo& createInfo);
		virtual ~RuntimeEngine();
	protected:
		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUpdate(float deltaTime) override;
	};

}