#include "FluxPCH.h"
#include "Shader.h"

#include "Flux/Core/Engine.h"

#include "OpenGL/OpenGLShader.h"

namespace Flux {

	Ref<Shader> Shader::Create(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLShader>::Create(vertexShaderSource, fragmentShaderSource);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}