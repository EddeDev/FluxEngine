#include "FluxPCH.h"
#include "OpenGLShader.h"

#include "Flux/Core/Engine.h"
#include "Flux/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	namespace Utils {

		static uint32 OpenGLShaderStage(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex: return GL_VERTEX_SHADER;
			case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
			case ShaderStage::Compute: return GL_COMPUTE_SHADER;
			}
			return 0;
		}

	}

	OpenGLShader::OpenGLShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		std::unordered_map<ShaderStage, std::string> sources;
		sources[ShaderStage::Vertex] = vertexShaderSource;
		sources[ShaderStage::Fragment] = fragmentShaderSource;
	
		Ref<OpenGLShader> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, sources]() mutable
		{
			std::vector<uint32> shaderIDs;
			for (auto& [stage, source] : sources)
			{
				uint32 shaderID = glCreateShader(Utils::OpenGLShaderStage(stage));

				const char* sourceCStr = source.c_str();
				glShaderSource(shaderID, 1, &sourceCStr, nullptr);

				glCompileShader(shaderID);

				int32 isCompiled;
				glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
				if (isCompiled == GL_FALSE)
				{
					FLUX_VERIFY(false);
					// TODO: handle
				}

				shaderIDs.push_back(shaderID);
			}

			instance->m_ProgramID = glCreateProgram();
			for (uint32 shaderID : shaderIDs)
				glAttachShader(instance->m_ProgramID, shaderID);

			glLinkProgram(instance->m_ProgramID);

			int32 isLinked;
			glGetProgramiv(instance->m_ProgramID, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE)
			{
				FLUX_VERIFY(false);
				// TODO: handle
			}

			for (uint32 shaderID : shaderIDs)
			{
				glDetachShader(instance->m_ProgramID, shaderID);
				glDeleteShader(shaderID);
			}
		});
	}

	OpenGLShader::~OpenGLShader()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([programID = m_ProgramID]() mutable
		{
			glDeleteProgram(programID);
		});
	}

	void OpenGLShader::Bind() const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const OpenGLShader> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]()
		{
			glUseProgram(instance->m_ProgramID);
		});
	}

	void OpenGLShader::Unbind() const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([]()
		{
			glUseProgram(0);
		});
	}

	void OpenGLShader::SetUniformMatrix4x4(const std::string& name, const float* data) const
	{
		float* storage = new float[4 * 4];
		memcpy(storage, data, sizeof(float) * 4 * 4);

		Ref<const OpenGLShader> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, name, storage]()
		{
			int32 location = glGetUniformLocation(instance->m_ProgramID, name.c_str());
			if (location == -1)
				__debugbreak();
			glUniformMatrix4fv(location, 1, GL_FALSE, storage);
			delete[] storage;
		});
	}

}