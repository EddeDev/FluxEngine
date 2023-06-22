#pragma once

namespace Flux {

	enum class ShaderStage : uint8
	{
		None = 0,

		Vertex,
		Fragment,
		Compute
	};

	enum class ShaderDataType : uint8
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		UInt, UInt2, UInt3, UInt4,
		Mat3, Mat4
	};

	struct VertexInputAttribute
	{
		std::string Name;
		uint32 Location = 0;
		uint32 Binding = 0;
		uint32 Size = 0;
		uint32 Offset = 0;
		ShaderDataType Type = ShaderDataType::None;
	};

	struct VertexInputLayout
	{
		uint32 Stride = 0;
		std::vector<VertexInputAttribute> Attributes;

		std::vector<VertexInputAttribute>::iterator begin() { return Attributes.begin(); }
		std::vector<VertexInputAttribute>::iterator end() { return Attributes.end(); }
		std::vector<VertexInputAttribute>::const_iterator begin() const { return Attributes.cbegin(); }
		std::vector<VertexInputAttribute>::const_iterator end() const { return Attributes.cend(); }
	};

	struct ShaderPushConstantMember
	{
		std::string Name;
		uint32 Size = 0;
		uint32 Offset = 0;
	};

	struct ShaderPushConstant
	{
		std::unordered_map<std::string, ShaderPushConstantMember> Members;
		std::string Name;
		uint32 Size = 0;
		uint32 Offset = 0;
		ShaderStage Stage = ShaderStage::None;
	};

	struct ShaderUniform
	{
		std::string Name;
		uint32 Size = 0;
		uint32 Offset = 0;
	};

	struct ShaderUniformBuffer
	{
		std::unordered_map<std::string, ShaderUniform> Uniforms;
		std::string Name;
		ShaderStage Stage = ShaderStage::None;
		uint32 Size = 0;
		uint32 Binding = 0;
		uint32 DescriptorSet = 0;
	};

	enum class ShaderDescriptorType : uint8
	{
		None = 0,

		UniformBuffer,
		StorageBuffer,
		CombinedImageSampler,
		SampledImage,
		StorageImage
	};

	struct ShaderDescriptor
	{
		std::string Name;
		ShaderDescriptorType Type = ShaderDescriptorType::None;
		ShaderStage Stage = ShaderStage::None;
		uint32 Count = 0;
		uint32 Binding = 0;
		uint32 DescriptorSet = 0;
	};

	typedef std::vector<uint32> ShaderBinary;
	typedef std::unordered_map<ShaderStage, ShaderBinary> ShaderBinaryMap;
	typedef std::unordered_map<ShaderStage, std::string> ShaderSourceMap;
	typedef std::unordered_map<ShaderStage, ShaderPushConstant> PushConstantMap;
	typedef std::map<uint32, std::map<ShaderDescriptorType, std::map<uint32, ShaderDescriptor>>> ShaderDescriptorSetMap;

	class Shader : public ReferenceCounted
	{
	public:
		virtual void Reload() = 0;

		virtual const VertexInputLayout& GetVertexInputLayout() const = 0;
		virtual const PushConstantMap& GetPushConstants() const = 0;
		virtual const ShaderDescriptorSetMap& GetDescriptorSets() const = 0;

		static Ref<Shader> Create(const std::filesystem::path& path);
	};

}