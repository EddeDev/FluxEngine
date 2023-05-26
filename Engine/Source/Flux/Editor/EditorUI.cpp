#include "FluxPCH.h"
#include "EditorUI.h"

#include "Flux/Runtime/Renderer/CommandBuffer.h"

namespace Flux {

	struct UIData
	{
		Ref<CommandBuffer> DrawCommandBuffer;
	};

	static UIData* s_Data = nullptr;

	void UI::Init(bool swapchainTarget)
	{
		s_Data = new UIData;
		memset(s_Data, 0, sizeof(UIData));

		CommandBufferCreateInfo commandBufferCreateInfo;
		commandBufferCreateInfo.CreateFromSwapchain = swapchainTarget;
		commandBufferCreateInfo.Transient = true;
		s_Data->DrawCommandBuffer = CommandBuffer::Create(commandBufferCreateInfo);
	}

	void UI::Shutdown()
	{
		delete s_Data;
		s_Data = nullptr;
	}

	void UI::BeginFrame()
	{
		FLUX_ASSERT(s_Data);
	}

	void UI::EndFrame()
	{
		FLUX_ASSERT(s_Data);
	}

}