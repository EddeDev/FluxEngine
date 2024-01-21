#pragma once

#include "Engine.h"

#include "Flux/Runtime/Renderer/VertexBuffer.h"
#include "Flux/Runtime/Renderer/IndexBuffer.h"
#include "Flux/Runtime/Renderer/Shader.h"
#include "Flux/Runtime/Renderer/GraphicsPipeline.h"
#include "Flux/Runtime/Renderer/Texture.h"

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
	private:
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<Shader> m_Shader;
		Ref<GraphicsPipeline> m_Pipeline;
	};

}