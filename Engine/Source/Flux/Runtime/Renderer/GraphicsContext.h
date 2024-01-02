#pragma once

#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	class GraphicsContext : public ReferenceCounted
	{
	public:
		virtual ~GraphicsContext() {}

		virtual bool Init() = 0;
		virtual void SwapBuffers(int32 swapInterval) = 0;

		static Ref<GraphicsContext> Create(WindowHandle windowHandle);
	};

}