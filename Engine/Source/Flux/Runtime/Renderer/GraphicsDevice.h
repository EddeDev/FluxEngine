#pragma once

#include "GraphicsContext.h"

namespace Flux {

	class GraphicsAdapter : public ReferenceCounted
	{
	public:
		virtual ~GraphicsAdapter() {}

		virtual Ref<GraphicsContext> GetContext() const = 0;

		static Ref<GraphicsAdapter> Create(Ref<GraphicsContext> context);
	};

	class GraphicsDevice : public ReferenceCounted
	{
	public:
		virtual ~GraphicsDevice() {}

		virtual Ref<GraphicsAdapter> GetAdapter() const = 0;

		static Ref<GraphicsDevice> Create(Ref<GraphicsAdapter> adapter);
	};

}
