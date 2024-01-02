#pragma once

#include "Flux/Runtime/Renderer/GraphicsContext.h"

namespace Flux {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(WindowHandle windowHandle);
		virtual ~OpenGLContext();

		virtual bool Init() override;
		virtual void SwapBuffers(int32 swapInterval) override;
	private:
		WindowHandle m_WindowHandle;
		HGLRC m_Context = NULL;
		HDC m_HDC = NULL;
		int32 m_SwapInterval = 1;
	};

}