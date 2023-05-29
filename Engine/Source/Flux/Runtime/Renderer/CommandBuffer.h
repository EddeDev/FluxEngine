#pragma once

namespace Flux {

	struct CommandBufferCreateInfo
	{
		uint32 Count = 1;
		bool Transient = true;
		bool CreateFromSwapchain = false;
		std::string DebugLabel;
	};

	class CommandBuffer : public ReferenceCounted
	{
	public:
		virtual ~CommandBuffer() {}

		virtual void Begin() = 0;
		virtual void RT_Begin() = 0;

		virtual void End() = 0;
		virtual void RT_End() = 0;

		virtual void Submit() = 0;
		virtual void RT_Submit() = 0;

		static Ref<CommandBuffer> Create(const CommandBufferCreateInfo& createInfo);
	};

}