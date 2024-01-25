#include "FluxPCH.h"
#include "OpenGLShader.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"
#include "Flux/Runtime/Utils/FileHelper.h"

#include <glad/glad.h>

namespace Flux {

	namespace Utils {

		static ShaderStage ShaderStageFromString(const std::string& string)
		{
			if (string == "vertex")
				return ShaderStage::Vertex;
			if (string == "fragment")
				return ShaderStage::Fragment;
			if (string == "compute")
				return ShaderStage::Compute;
			return ShaderStage::None;
		}

		static uint32 OpenGLShaderStage(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex: return GL_VERTEX_SHADER;
			case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
			case ShaderStage::Compute: return GL_COMPUTE_SHADER;
			}
			FLUX_VERIFY(false, "Unknown shader stage");
			return 0;
		}

	}

	static ShaderSourceMap PreProcess(const std::string& source)
	{
		ShaderSourceMap sources;

		const char* stageToken = "#stage";
		size_t stageTokenLength = strlen(stageToken);

		size_t stageTokenPos = source.find(stageToken);
		while (stageTokenPos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", stageTokenPos);
			FLUX_VERIFY(eol != std::string::npos, "Syntax error");

			size_t begin = stageTokenPos + stageTokenLength + 1;

			std::string stageName = source.substr(begin, eol - begin);
			ShaderStage stage = Utils::ShaderStageFromString(stageName);

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			stageTokenPos = source.find(stageToken, nextLinePos);

			sources[stage] = source.substr(nextLinePos, stageTokenPos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

			if (stage == ShaderStage::Compute)
				break;
		}

		return sources;
	}

	OpenGLShader::OpenGLShader(const std::filesystem::path& path)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLShaderData();

		std::string source;
		FLUX_VERIFY(FileHelper::LoadFileToString(source, path), "Failed to load shader: {0}", path.string());

		ShaderSourceMap sources = PreProcess(source);
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

	OpenGLShader::OpenGLShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLShaderData();

		ShaderSourceMap sources;
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

	void OpenGLShader::SetUniform(const std::string& name, const Matrix4x4& value) const
	{
		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, name, value]()
		{
			int32 location = glGetUniformLocation(data->ProgramID, name.c_str());
			if (location == -1)
				__debugbreak();
			glUniformMatrix4fv(location, 1, GL_FALSE, value.GetFloatPointer());
		});
	}

}