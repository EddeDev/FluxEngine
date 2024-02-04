#pragma once

#include "VertexDeclaration.h"

#include "IndexBuffer.h"

namespace Flux {

	enum class PrimitiveTopology : uint8
	{
		None = 0,

		Triangles
	};

	enum class IndexFormat : uint8
	{
		UInt32 = 0,
		UInt16,
		UInt8,
	};

	struct GraphicsPipelineCreateInfo
	{
		Flux::VertexDeclaration VertexDeclaration;

		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		bool DepthTest = true;
		bool ScissorTest = false;
		bool DepthWrite = true;
		bool BackfaceCulling = false;
	};

	class GraphicsPipeline : public ReferenceCounted
	{
	public:
		virtual ~GraphicsPipeline() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Scissor(int32 x, int32 y, int32 width, int32 height) const = 0;

		virtual void DrawIndexed(IndexFormat indexFormat, uint32 indexCount, uint32 startIndexLocation = 0, uint32 baseVertexLocation = 0) const = 0;

		static Ref<GraphicsPipeline> Create(const GraphicsPipelineCreateInfo& createInfo);
	};

	namespace Utils {

		inline static uint32 IndexFormatSize(IndexFormat format)
		{
			switch (format)
			{
			case IndexFormat::UInt32: return sizeof(uint32);
			case IndexFormat::UInt16: return sizeof(uint16);
			case IndexFormat::UInt8:  return sizeof(uint8);
			}
			FLUX_VERIFY(false, "Unknown index format!");
			return 0;
		}

		inline static const char* IndexFormatToString(IndexFormat format)
		{
			switch (format)
			{
			case IndexFormat::UInt32: return "UInt32";
			case IndexFormat::UInt16: return "UInt16";
			case IndexFormat::UInt8:  return "UInt8";
			}
			FLUX_VERIFY(false, "Unknown index format!");
			return 0;
		}

	}

}