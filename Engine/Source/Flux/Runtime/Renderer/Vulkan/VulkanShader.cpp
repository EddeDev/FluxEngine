#include "FluxPCH.h"
#include "VulkanShader.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <shaderc/shaderc.hpp>

namespace Flux {

	namespace Utils {

		static bool LoadFileToString(std::string& outString, const std::filesystem::path& path)
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

		static ShaderStage ShaderStageFromString(const std::string& stageName)
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

		static const char* ShaderStageToString(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:   return "Vertex";
			case ShaderStage::Fragment: return "Fragment";
			case ShaderStage::Compute:  return "Compute";
			}
			FLUX_VERIFY(false, "Unknown shader stage");
			return "";
		}

		static shaderc_shader_kind ShaderStageToShaderC(ShaderStage stage)
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

		static VkShaderStageFlags VulkanShaderStage(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case ShaderStage::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
			}
			FLUX_VERIFY(false, "Unknown shader stage");
			return static_cast<VkShaderStageFlagBits>(0);
		}

		static VkDescriptorType VulkanDescriptorType(DescriptorType type)
		{
			switch (type)
			{
			case DescriptorType::UniformBuffer:        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case DescriptorType::StorageBuffer:        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case DescriptorType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case DescriptorType::SampledImage:         return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case DescriptorType::StorageImage:         return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			}
			FLUX_VERIFY(false, "Unknown descriptor type");
			return static_cast<VkDescriptorType>(0);
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
		FLUX_CHECK_IS_MAIN_THREAD();

		Reload();
	}

	VulkanShader::~VulkanShader()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([shaderModules = m_ShaderModules, descriptorSetLayouts = m_DescriptorSetLayouts]()
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();

			for (auto& [set, descriptorSetLayout] : descriptorSetLayouts)
				vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

			for (auto& [stage, shaderModule] : shaderModules)
				vkDestroyShaderModule(device, shaderModule, nullptr);
		});
	}

	void VulkanShader::Reload()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Binaries.clear();
		m_VertexInputLayout = {};
		m_PushConstants.clear();
		m_DescriptorSetLayouts.clear();
		m_DescriptorPoolSizes.clear();
		m_DescriptorSetLayoutBindings.clear();

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
		CreateDescriptors();

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

// #ifdef FLUX_BUILD_DEBUG
		compileOptions.SetGenerateDebugInfo();
// #endif

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
		FLUX_CHECK_IS_RENDER_THREAD();

		VkDevice device = VulkanDevice::Get()->GetDevice();

		for (const auto& [stage, binary] : m_Binaries)
		{
			auto it = m_ShaderModules.find(stage);
			if (it != m_ShaderModules.end())
				vkDestroyShaderModule(device, it->second, nullptr);

			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = binary.size() * sizeof(uint32);
			createInfo.pCode = binary.data();

			VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &m_ShaderModules[stage]));
		}
	}

	void VulkanShader::Reflect()
	{
		FLUX_TRACE("Shader Reflection - {0}", m_Path.string());

		for (const auto& [stage, binary] : m_Binaries)
		{
			FLUX_TRACE("  {0} shader", Utils::ShaderStageToString(stage));

			spirv_cross::Compiler compiler(binary);
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();

			if (stage == ShaderStage::Vertex)
			{
				for (const auto& resource : resources.stage_inputs)
				{
					auto& attribute = m_VertexInputLayout.Attributes.emplace_back();
					auto& type = compiler.get_type(resource.base_type_id);

					attribute.Name = compiler.get_name(resource.id);
					if (attribute.Name.empty())
						attribute.Name = compiler.get_fallback_name(resource.id);

					attribute.Location = compiler.get_decoration(resource.id, spv::DecorationLocation);
					attribute.Binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
					attribute.Type = Utils::SPIRTypeToShaderDataType(type);
					attribute.Size = Utils::SPIRTypeSize(type);
				}

				std::sort(m_VertexInputLayout.Attributes.begin(), m_VertexInputLayout.Attributes.end(), [](const auto& a, const auto& b)
				{
					return a.Location < b.Location;
				});

				uint32 stride = 0;
				for (size_t i = 0; i < resources.stage_inputs.size(); i++)
				{
					auto& attribute = m_VertexInputLayout.Attributes[i];
					attribute.Offset = stride;
					stride += attribute.Size;
				}

				m_VertexInputLayout.Stride = stride;
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
				pushConstant.Size = static_cast<uint32>(compiler.get_declared_struct_size(bufferType));

				FLUX_TRACE("    Push constant: {0} (size = {1}, offset = {2})", pushConstant.Name, pushConstant.Size, pushConstant.Offset);

				for (uint32 i = 0; i < static_cast<uint32>(bufferType.member_types.size()); i++)
				{
					std::string memberName = compiler.get_member_name(bufferType.self, i);
					if (memberName.empty())
						memberName = compiler.get_fallback_member_name(i);
					FLUX_ASSERT(!memberName.empty());

					std::string fullName = fmt::format("{}.{}", pushConstant.Name, memberName);

					auto& member = pushConstant.Members[fullName];
					member.Name = memberName;
					member.Size = static_cast<uint32>(compiler.get_declared_struct_member_size(bufferType, i));
					member.Offset = compiler.type_struct_member_offset(bufferType, i);
				}
			}
			
			if (!resources.uniform_buffers.empty() || !resources.storage_buffers.empty())
				FLUX_TRACE("    Buffers:");

			for (const auto& resource : resources.uniform_buffers)
			{
				auto& bufferType = compiler.get_type(resource.base_type_id);

				std::string bufferName = compiler.get_name(resource.base_type_id);
				if (bufferName.empty())
					bufferName = compiler.get_fallback_name(resource.base_type_id);

				uint32 descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

				uint32 bufferSize = static_cast<uint32>(compiler.get_declared_struct_size(bufferType));

				const auto descriptorType = DescriptorType::UniformBuffer;

				auto& descriptor = m_DescriptorSets[descriptorSet][descriptorType][binding];
				descriptor.Name = bufferName;
				descriptor.Type = descriptorType;
				descriptor.Stage = stage;
				descriptor.Count = 1;
				descriptor.Binding = binding;
				descriptor.DescriptorSet = descriptorSet;

				FLUX_TRACE("      {0} (set = {1}, binding = {2})", bufferName, descriptorSet, binding);

				ShaderUniformBuffer buffer;
				buffer.Name = bufferName;
				buffer.Stage = stage;
				buffer.Size = bufferSize;
				buffer.Binding = binding;

				for (uint32 i = 0; i < static_cast<uint32>(bufferType.member_types.size()); i++)
				{
					std::string uniformName = compiler.get_member_name(bufferType.self, i);
					if (uniformName.empty())
						uniformName = compiler.get_fallback_member_name(i);

					uint32 size = static_cast<uint32>(compiler.get_declared_struct_member_size(bufferType, i));
					uint32 offset = compiler.type_struct_member_offset(bufferType, i);

					auto& uniform = buffer.Uniforms[uniformName];
					uniform.Name = uniformName;
					uniform.Size = size;
					uniform.Offset = offset;
				}

				Renderer::RegisterUniformBuffer(buffer);
			}

			for (const auto& resource : resources.storage_buffers)
			{
				std::string name = compiler.get_name(resource.base_type_id);
				if (name.empty())
					name = compiler.get_fallback_name(resource.base_type_id);

				uint32 descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

				const auto descriptorType = DescriptorType::StorageBuffer;

				auto& descriptor = m_DescriptorSets[descriptorSet][descriptorType][binding];
				descriptor.Name = name;
				descriptor.Type = descriptorType;
				descriptor.Stage = stage;
				descriptor.Count = 1;
				descriptor.Binding = binding;
				descriptor.DescriptorSet = descriptorSet;

				FLUX_TRACE("      {0} (set = {1}, binding = {2})", name, descriptorSet, binding);
			}

			if (!resources.sampled_images.empty() || !resources.storage_images.empty())
				FLUX_TRACE("    Resources:");

			for (const auto& resource : resources.sampled_images)
			{
				auto& type = compiler.get_type(resource.type_id);

				uint32 arraySize = type.array[0];
				if (arraySize == 0)
					arraySize = 1;

				std::string name = compiler.get_name(resource.id);
				if (name.empty())
					name = compiler.get_fallback_name(resource.id);

				uint32 descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

				const auto descriptorType = DescriptorType::CombinedImageSampler;

				auto& descriptor = m_DescriptorSets[descriptorSet][descriptorType][binding];
				descriptor.Name = name;
				descriptor.Type = descriptorType;
				descriptor.Stage = stage;
				descriptor.Count = arraySize;
				descriptor.Binding = binding;
				descriptor.DescriptorSet = descriptorSet;

				FLUX_TRACE("      {0} (set = {1}, binding = {2}, array size = {3})", name, descriptorSet, binding, arraySize);
			}

			for (const auto& resource : resources.separate_images)
			{
				auto& type = compiler.get_type(resource.type_id);

				uint32 arraySize = type.array[0];
				if (arraySize == 0)
					arraySize = 1;

				std::string name = compiler.get_name(resource.id);
				if (name.empty())
					name = compiler.get_fallback_name(resource.id);

				uint32 descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

				const auto descriptorType = DescriptorType::SampledImage;

				auto& descriptor = m_DescriptorSets[descriptorSet][descriptorType][binding];
				descriptor.Name = name;
				descriptor.Type = descriptorType;
				descriptor.Stage = stage;
				descriptor.Count = arraySize;
				descriptor.Binding = binding;
				descriptor.DescriptorSet = descriptorSet;

				FLUX_TRACE("      {0} (set = {1}, binding = {2}, array size = {3})", name, descriptorSet, binding, arraySize);
			}

			for (const auto& resource : resources.storage_images)
			{
				auto& type = compiler.get_type(resource.type_id);

				uint32 arraySize = type.array[0];
				if (arraySize == 0)
					arraySize = 1;

				std::string name = compiler.get_name(resource.id);
				if (name.empty())
					name = compiler.get_fallback_name(resource.id);

				uint32 descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

				const auto descriptorType = DescriptorType::StorageImage;

				auto& descriptor = m_DescriptorSets[descriptorSet][descriptorType][binding];
				descriptor.Name = name;
				descriptor.Type = descriptorType;
				descriptor.Stage = stage;
				descriptor.Count = arraySize;
				descriptor.Binding = binding;
				descriptor.DescriptorSet = descriptorSet;

				FLUX_TRACE("      {0} (set = {1}, binding = {2}, array size = {3})", name, descriptorSet, binding, arraySize);
			}
		}
	}

	void VulkanShader::CreateDescriptors()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		for (const auto& [set, descriptorSet] : m_DescriptorSets)
		{
			for (const auto& [shaderDescriptorType, descriptors] : descriptorSet)
			{
				const VkDescriptorType descriptorType = Utils::VulkanDescriptorType(shaderDescriptorType);

				m_DescriptorPoolSizes[set].push_back({
					descriptorType,
					static_cast<uint32>(descriptors.size())
				});

				for (const auto& [binding, descriptor] : descriptors)
				{
					FLUX_VERIFY(descriptor.Type == shaderDescriptorType);

					if (shaderDescriptorType == DescriptorType::None)
					{
						FLUX_VERIFY(false, "Unknown descriptor type!");
						continue;
					}

					m_DescriptorSetLayoutBindings[set].push_back({
						binding,
						descriptorType,
						descriptor.Count,
						Utils::VulkanShaderStage(descriptor.Stage)
					});
				}
			}
		}

		Ref<VulkanShader> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();

			// TODO: synchronize access
			for (const auto& [set, layoutBinding] : instance->m_DescriptorSetLayoutBindings)
			{
				VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
				descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32>(layoutBinding.size());
				descriptorSetLayoutCreateInfo.pBindings = layoutBinding.data();

				VK_CHECK(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &instance->m_DescriptorSetLayouts[set]));
			}
		});
	}

	VkDescriptorSet VulkanShader::RT_CreateDescriptorSet(uint32 set) const
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		FLUX_VERIFY(m_DescriptorPoolSizes.find(set) != m_DescriptorPoolSizes.end());
		FLUX_VERIFY(m_DescriptorSetLayouts.find(set) != m_DescriptorSetLayouts.end());

		VkDevice device = VulkanDevice::Get()->GetDevice();

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets = 1;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(m_DescriptorPoolSizes.at(set).size());
		descriptorPoolCreateInfo.pPoolSizes = m_DescriptorPoolSizes.at(set).data();

		VkDescriptorPool descriptorPool;
		VK_CHECK(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));

		VkDescriptorSetAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = descriptorPool;
		allocateInfo.descriptorSetCount = 1;
		allocateInfo.pSetLayouts = &m_DescriptorSetLayouts.at(set);

		VkDescriptorSet descriptorSet;
		VK_CHECK(vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet));
		return descriptorSet;
	}

}