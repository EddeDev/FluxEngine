#include "FluxPCH.h"
#include "VulkanShader.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <shaderc/shaderc.hpp>

namespace Flux {

	namespace Utils {

		bool LoadFileToString(std::string& outString, const std::filesystem::path& path)
		{
			std::ifstream in(path, std::ios::in | std::ios::binary);
			if (!in)
				return false;

			in.seekg(0, std::ios::end);
			std::streampos size = in.tellg();
			in.seekg(0, std::ios::beg);
			outString.resize(size);
			in.read(outString.data(), size);

			in.close();
			return true;
		}

		ShaderStage ShaderStageFromString(const std::string& stageName)
		{
			if (stageName == "vertex")
				return ShaderStage::Vertex;
			if (stageName == "fragment")
				return ShaderStage::Fragment;
			if (stageName == "compute")
				return ShaderStage::Compute;
			FLUX_VERIFY(false, "Unknown shader stage");
			return ShaderStage::None;
		}

		shaderc_shader_kind ShaderStageToShaderC(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:   return shaderc_vertex_shader;
			case ShaderStage::Fragment: return shaderc_fragment_shader;
			case ShaderStage::Compute:  return shaderc_compute_shader;
			}
			FLUX_VERIFY(false, "Unknown shader stage");
			return static_cast<shaderc_shader_kind>(0);
		}

	}

	static std::unordered_map<ShaderStage, std::string> Preprocess(const std::string& source)
	{
		std::unordered_map<ShaderStage, std::string> sources;

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

	VulkanShader::VulkanShader(const std::filesystem::path& path)
		: m_Path(path)
	{
		Reload();
	}

	VulkanShader::~VulkanShader()
	{
		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([shaderModules = m_ShaderModules]()
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();
			for (auto& [stage, shaderModule] : shaderModules)
				vkDestroyShaderModule(device, shaderModule, nullptr);
		});
	}

	void VulkanShader::Reload()
	{
		m_Binaries.clear();

		std::string source;
		if (!Utils::LoadFileToString(source, m_Path))
		{
			FLUX_WARNING("Failed to load shader: {0}", m_Path.string());
			return;
		}

		ShaderSourceMap sources = Preprocess(source);
		if (sources.empty())
		{
			FLUX_VERIFY(false);
			return;
		}

		m_Binaries = LoadOrCompileBinaries(sources);
		if (m_Binaries.empty())
		{
			FLUX_VERIFY(false);
			return;
		}

		Ref<VulkanShader> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_CreateShaders();
		});
	}

	ShaderBinaryMap VulkanShader::LoadOrCompileBinaries(const ShaderSourceMap& sources)
	{
		ShaderBinaryMap binaries;

		// TODO: cache shaders
		for (auto& [stage, source] : sources)
		{
			shaderc::CompileOptions compileOptions;
			compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
			compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
			compileOptions.SetWarningsAsErrors();

#ifdef FLUX_BUILD_DEBUG
			compileOptions.SetGenerateDebugInfo();
#endif


			shaderc::Compiler compiler;
			shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, Utils::ShaderStageToShaderC(stage), m_Path.string().c_str(), compileOptions);
			if (result.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				FLUX_ERROR("Failed to compile shader: {0}\n{1}", m_Path.string(), result.GetErrorMessage());
				// FLUX_VERIFY(false);
			}

			binaries[stage] = { result.begin(), result.end() };
		}

		return binaries;
	}

	void VulkanShader::RT_CreateShaders()
	{
		VkDevice device = VulkanDevice::Get()->GetDevice();

		for (auto&& [stage, binary] : m_Binaries)
		{
			auto it = m_ShaderModules.find(stage);
			if (it != m_ShaderModules.end())
			{
				vkDestroyShaderModule(device, it->second, nullptr);
				m_ShaderModules.erase(it);
			}

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = binary.size() * sizeof(uint32);
			createInfo.pCode = binary.data();

			VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &m_ShaderModules[stage]));
		}
	}

}