#include "FluxPCH.h"
#include "Shader.h"

#include "Flux/Runtime/Core/Engine.h"

#include "OpenGL/OpenGLShader.h"

namespace Flux {

	Ref<Shader> Shader::Create(const std::filesystem::path& path)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLShader>::Create(path);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

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