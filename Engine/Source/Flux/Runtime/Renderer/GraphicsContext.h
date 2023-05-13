#pragma once

namespace Flux {

	class GraphicsContext : public ReferenceCounted
	{
	public:
		virtual ~GraphicsContext() {}

		static Ref<GraphicsContext> Create();
	};

}