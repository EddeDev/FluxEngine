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
			}
			FLUX_VERIFY(false, "Unknown vertex element format!");
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
						GL_FALSE,
						stride,
						(const void*)element.Offset
					);
				}
				else
				{
					glVertexAttribIPointer(
						i,
						element.ComponentCount,
						type,
						stride,
						(const void*)element.Offset
					);
				}
			}
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

			glBindVertexArray(0);
		});
	}

}