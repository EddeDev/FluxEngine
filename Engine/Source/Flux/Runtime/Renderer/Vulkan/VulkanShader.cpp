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

		static ShaderDataType SPIRTypeToShaderDataType(const spirv_cross::SPIRType& type)
		{
			switch (type.basetype)
			{
			case spirv_cross::SPIRType::Float:
			{
				switch (type.vecsize)
				{
				case 1: return ShaderDataType::Float;
				case 2: return ShaderDataType::Float2;
				case 3: return ShaderDataType::Float3;
				case 4: return ShaderDataType::Float4;
				}
				break;
			}
			case spirv_cross::SPIRType::Int:
			{
				switch (type.vecsize)
				{
				case 1: return ShaderDataType::Int;
				case 2: return ShaderDataType::Int2;
				case 3: return ShaderDataType::Int3;
				case 4: return ShaderDataType::Int4;
				}
				break;
			}
			case spirv_cross::SPIRType::UInt:
			{
				switch (type.vecsize)
				{
				case 1: return ShaderDataType::UInt;
				case 2: return ShaderDataType::UInt2;
				case 3: return ShaderDataType::UInt3;
				case 4: return ShaderDataType::UInt4;
				}
				break;
			}
			}
			FLUX_VERIFY(false);
			return ShaderDataType::None;
		}

		static uint32 SPIRTypeSize(const spirv_cross::SPIRType& type)
		{
			switch (type.basetype)
			{
			case spirv_cross::SPIRType::Float: return sizeof(float) * type.vecsize;
			case spirv_cross::SPIRType::Int:   return sizeof(int32) * type.vecsize;
			case spirv_cross::SPIRType::UInt:  return sizeof(uint32) * type.vecsize;
			}
			FLUX_VERIFY(false);
			return 0;
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
		m_InputLayout = {};
		m_PushConstants = {};

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

		Reflect();

		Ref<VulkanShader> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_CreateShaders();
		});
	}

	ShaderBinaryMap VulkanShader::LoadOrCompileBinaries(const ShaderSourceMap& sources)
	{
		ShaderBinaryMap binaries;

		shaderc::Compiler compiler;
		shaderc::CompileOptions compileOptions;
		compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
		compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
		compileOptions.SetWarningsAsErrors();

#ifdef FLUX_BUILD_DEBUG
		compileOptions.SetGenerateDebugInfo();
#endif

		// TODO: cache shaders
		for (auto& [stage, source] : sources)
		{
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

		for (const auto& [stage, binary] : m_Binaries)
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

	void VulkanShader::Reflect()
	{
		for (const auto& [stage, binary] : m_Binaries)
		{
			spirv_cross::Compiler compiler(binary);
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();

			if (stage == ShaderStage::Vertex)
			{
				uint32 offset = 0;

				for (const auto& resource : resources.stage_inputs)
				{
					auto& attribute = m_InputLayout.Attributes.emplace_back();
					auto& type = compiler.get_type(resource.base_type_id);

					attribute.Name = compiler.get_name(resource.id);
					if (attribute.Name.empty())
						attribute.Name = compiler.get_fallback_name(resource.id);

					attribute.Location = compiler.get_decoration(resource.id, spv::DecorationLocation);
					attribute.Binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
					attribute.Offset = offset;
					attribute.Type = Utils::SPIRTypeToShaderDataType(type);

					offset += Utils::SPIRTypeSize(type);
				}

				m_InputLayout.Stride = offset;
			}

			for (const auto& resource : resources.push_constant_buffers)
			{
				auto& pushConstant = m_PushConstants[stage];
				pushConstant.Stage = stage;
				pushConstant.Offset = compiler.get_decoration(resource.id, spv::DecorationOffset);

				pushConstant.Name = compiler.get_name(resource.id);
				if (pushConstant.Name.empty())
					pushConstant.Name = compiler.get_fallback_name(resource.id);

				const auto& bufferType = compiler.get_type(resource.base_type_id);
				pushConstant.Size = compiler.get_declared_struct_size(bufferType);

				for (uint32 i = 0; i < static_cast<uint32>(bufferType.member_types.size()); i++)
				{
					std::string memberName = compiler.get_member_name(bufferType.self, i);
					if (memberName.empty())
						memberName = compiler.get_fallback_member_name(i);
					FLUX_ASSERT(!memberName.empty());

					std::string fullName = fmt::format("{}.{}", pushConstant.Name, memberName);

					auto& member = pushConstant.Members[fullName];
					member.Name = memberName;
					member.Size = compiler.get_declared_struct_member_size(bufferType, i);
					member.Offset = compiler.type_struct_member_offset(bufferType, i);
				}
			}
		}
	}

}