#include "FluxPCH.h"
#include "OpenGLShader.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

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
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLShaderData();

		std::unordered_map<ShaderStage, std::string> sources;
		sources[ShaderStage::Vertex] = vertexShaderSource;
		sources[ShaderStage::Fragment] = fragmentShaderSource;
	
		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, sources]() mutable
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

			data->ProgramID = glCreateProgram();
			for (uint32 shaderID : shaderIDs)
				glAttachShader(data->ProgramID, shaderID);

			glLinkProgram(data->ProgramID);

			int32 isLinked;
			glGetProgramiv(data->ProgramID, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE)
			{
				FLUX_VERIFY(false);
				// TODO: handle
			}

			for (uint32 shaderID : shaderIDs)
			{
				glDetachShader(data->ProgramID, shaderID);
				glDeleteShader(shaderID);
			}
		});
	}

	OpenGLShader::~OpenGLShader()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([data = m_Data]()
		{
			if (data->ProgramID)
				glDeleteProgram(data->ProgramID);
			delete data;
		});
	}

	void OpenGLShader::Bind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data]()
		{
			glUseProgram(data->ProgramID);
		});
	}

	void OpenGLShader::Unbind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([]()
		{
			glUseProgram(0);
		});
	}

	void OpenGLShader::SetUniformMatrix4x4(const std::string& name, const float* data) const
	{
		float* storage = new float[4 * 4];
		memcpy(storage, data, sizeof(float) * 4 * 4);

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, name, storage]()
		{
			int32 location = glGetUniformLocation(data->ProgramID, name.c_str());
			if (location == -1)
				__debugbreak();
			glUniformMatrix4fv(location, 1, GL_FALSE, storage);
			delete[] storage;
		});
	}

}