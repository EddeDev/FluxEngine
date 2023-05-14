#pragma once

#include "GraphicsContext.h"

namespace Flux {

	class GraphicsAdapter : public ReferenceCounted
	{
	public:
		virtual ~GraphicsAdapter() {}

		static Ref<GraphicsAdapter> Create(Ref<GraphicsContext> context);
	};

	class GraphicsDevice : public ReferenceCounted
	{
	public:
		virtual ~GraphicsDevice() {}

		static Ref<GraphicsDevice> Create(Ref<GraphicsAdapter> adapter);
	};

}
