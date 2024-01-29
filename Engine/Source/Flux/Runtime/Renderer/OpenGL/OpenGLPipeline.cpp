#include "FluxPCH.h"
#include "OpenGLPipeline.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

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

	}

	OpenGLPipeline::OpenGLPipeline(const GraphicsPipelineCreateInfo& createInfo)
		: m_Topology(createInfo.Topology)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLPipelineData();
		m_Data->CreateInfo = createInfo;

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data]() mutable
		{
			glCreateVertexArrays(1, &data->VertexArrayID);
		});
	}

	OpenGLPipeline::~OpenGLPipeline()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([data = m_Data]()
		{
			if (data->VertexArrayID)
				glDeleteVertexArrays(1, &data->VertexArrayID);
			delete data;
		});
	}

	void OpenGLPipeline::Bind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();
	
		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data]()
		{
			glBindVertexArray(data->VertexArrayID);

			const auto& createInfo = data->CreateInfo;
			const auto& elements = createInfo.VertexDeclaration.GetElements();

			uint32 stride = createInfo.VertexDeclaration.GetStride();

			for (uint32 i = 0; i < static_cast<uint32>(elements.size()); i++)
			{
				const auto& element = elements[i];

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

			if (createInfo.BackfaceCulling)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}

			if (createInfo.DepthTest)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);

			if (createInfo.ScissorTest)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);

			glDepthMask(createInfo.DepthWrite);

			glFrontFace(GL_CW);
			glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

			glDisable(GL_STENCIL_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		});
	}

	void OpenGLPipeline::Unbind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data]()
		{
			const auto& createInfo = data->CreateInfo;
			const auto& elements = createInfo.VertexDeclaration.GetElements();
			for (uint32 i = 0; i < static_cast<uint32>(elements.size()); i++)
				glDisableVertexAttribArray(i);

			glDisable(GL_SCISSOR_TEST);

			glBindVertexArray(0);
		});
	}

	void OpenGLPipeline::Scissor(int32 x, int32 y, int32 width, int32 height) const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([x, y, width, height]()
		{
			glScissor(x, y, width, height);
		});
	}

	void OpenGLPipeline::DrawIndexed(IndexBufferDataType dataType, uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, topology = m_Topology, dataType, indexCount, startIndexLocation, baseVertexLocation]()
		{
			glDrawElementsBaseVertex(
				Utils::OpenGLPrimitiveTopology(topology),
				indexCount,
				Utils::OpenGLIndexBufferDataType(dataType),
				(const void*)(intptr)(startIndexLocation * Utils::IndexBufferDataTypeSize(dataType)),
				baseVertexLocation
			);
		});
	}

}