#include "FluxPCH.h"
#include "OpenGLPipeline.h"

#include "Flux/Core/Engine.h"
#include "Flux/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	namespace Utils {

		static uint32 OpenGLVertexElementFormat(VertexElementFormat format)
		{
			switch (format)
			{
			case VertexElementFormat::Float:  return GL_FLOAT;
			case VertexElementFormat::Float2: return GL_FLOAT;
			case VertexElementFormat::Float3: return GL_FLOAT;
			case VertexElementFormat::Float4: return GL_FLOAT;
			case VertexElementFormat::UByte:  return GL_UNSIGNED_BYTE;
			case VertexElementFormat::UByte2: return GL_UNSIGNED_BYTE;
			case VertexElementFormat::UByte3: return GL_UNSIGNED_BYTE;
			case VertexElementFormat::UByte4: return GL_UNSIGNED_BYTE;
			}
			FLUX_VERIFY(false, "Unknown vertex element format!");
			return 0;
		}

		static uint32 OpenGLPrimitiveTopology(PrimitiveTopology topology)
		{
			switch (topology)
			{
			case PrimitiveTopology::Triangles: return GL_TRIANGLES;
			}
			FLUX_VERIFY(false, "Unknown primitive topology!");
			return 0;
		}

		static uint32 OpenGLIndexBufferDataType(IndexBufferDataType dataType)
		{
			switch (dataType)
			{
			case IndexBufferDataType::UInt32: return GL_UNSIGNED_INT;
			case IndexBufferDataType::UInt16: return GL_UNSIGNED_SHORT;
			case IndexBufferDataType::UInt8:  return GL_UNSIGNED_BYTE;
			}
			FLUX_VERIFY(false, "Unknown index buffer data type!");
			return 0;
		}

		static uint32 IndexBufferDataTypeSize(IndexBufferDataType dataType)
		{
			switch (dataType)
			{
			case IndexBufferDataType::UInt32: return sizeof(uint32);
			case IndexBufferDataType::UInt16: return sizeof(uint16);
			case IndexBufferDataType::UInt8:  return sizeof(uint8);
			}
			FLUX_VERIFY(false, "Unknown index buffer data type!");
			return 0;
		}

	}

	OpenGLPipeline::OpenGLPipeline(const GraphicsPipelineCreateInfo& createInfo)
		: m_CreateInfo(createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<OpenGLPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			glCreateVertexArrays(1, &instance->m_VertexArrayID);
		});
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([vertexArrayID = m_VertexArrayID]() mutable
		{
			glDeleteVertexArrays(1, &vertexArrayID);
		});
	}

	void OpenGLPipeline::Bind() const
	{
		FLUX_CHECK_IS_MAIN_THREAD();
	
		Ref<const OpenGLPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]()
		{
			glBindVertexArray(instance->m_VertexArrayID);

			auto& elements = instance->m_CreateInfo.VertexDeclaration.GetElements();

			uint32 stride = instance->m_CreateInfo.VertexDeclaration.GetStride();

			for (uint32 i = 0; i < static_cast<uint32>(elements.size()); i++)
			{
				auto& element = elements[i];

				glEnableVertexAttribArray(i);

				uint32 type = Utils::OpenGLVertexElementFormat(element.Format);
				if (type != GL_INT)
				{
					glVertexAttribPointer(
						i,
						element.ComponentCount,
						type,
						element.Normalized ? GL_TRUE : GL_FALSE,
						stride,
						(const void*)(intptr)element.Offset
					);
				}
				else
				{
					glVertexAttribIPointer(
						i,
						element.ComponentCount,
						type,
						stride,
						(const void*)(intptr)element.Offset
					);
				}
			}

			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);
			glEnable(GL_SCISSOR_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		});
	}

	void OpenGLPipeline::Unbind() const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const OpenGLPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]()
		{
			auto& elements = instance->m_CreateInfo.VertexDeclaration.GetElements();
			for (uint32 i = 0; i < static_cast<uint32>(elements.size()); i++)
				glDisableVertexAttribArray(i);

			glDisable(GL_SCISSOR_TEST);

			glBindVertexArray(0);
		});
	}

	void OpenGLPipeline::Scissor(int32 x, int32 y, int32 width, int32 height) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([x, y, width, height]()
		{
			glScissor(x, y, width, height);
		});
	}

	void OpenGLPipeline::DrawIndexed(IndexBufferDataType dataType, uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const OpenGLPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, dataType, indexCount, startIndexLocation, baseVertexLocation]()
		{
			glDrawElementsBaseVertex(
				Utils::OpenGLPrimitiveTopology(instance->m_CreateInfo.Topology),
				indexCount,
				Utils::OpenGLIndexBufferDataType(dataType),
				(const void*)(intptr)(startIndexLocation * Utils::IndexBufferDataTypeSize(dataType)),
				baseVertexLocation
			);
		});
	}

}