#pragma once

namespace Flux {

	class Swapchain : public ReferenceCounted
	{
	public:
		virtual ~Swapchain() {}

		virtual void BeginFrame() = 0;
		virtual void Present(int32 swapInterval) = 0;

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;

		static Ref<Swapchain> Create(Window* window);
	};

}