#pragma once

#include "Flux/Runtime/Renderer/GraphicsContext.h"

#include "DX11.h"

namespace Flux {

	class DX11Context : public GraphicsContext
	{
	public:
		DX11Context(WindowHandle windowHandle);
		virtual ~DX11Context();

		virtual bool Init() override;
		virtual void SwapBuffers(int32 swapInterval) override;
	private:
		WindowHandle m_WindowHandle;

		DXRef<IDXGIAdapter> m_Adapter;
	};

}