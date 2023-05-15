#pragma once

namespace Flux {

	class Swapchain : public ReferenceCounted
	{
	public:
		virtual ~Swapchain() {}

		virtual void BeginFrame() = 0;
		virtual void Present() = 0;

		static Ref<Swapchain> Create(Window* window);
	};

}