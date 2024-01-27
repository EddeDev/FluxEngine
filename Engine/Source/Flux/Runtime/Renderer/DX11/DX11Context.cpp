#include "FluxPCH.h"
#include "DX11Context.h"

#include "Flux/Runtime/Core/Engine.h"

namespace Flux {

	namespace Utils {

		static std::vector<DXRef<IDXGIAdapter>> GetAdapters()
		{
			DXRef<IDXGIFactory> factory;
			if (FAILED(CreateDXGIFactory(DX_RIID(IDXGIFactory), &factory)))
			{
				FLUX_VERIFY(false, "CreateDXGIFactory failed");
				return {};
			}

			std::vector<DXRef<IDXGIAdapter>> adapters;
			DXRef<IDXGIAdapter> adapter;
			for (uint32 i = 0; factory->EnumAdapters(i, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; i++)
				adapters.push_back(adapter);
			return adapters;
		}

	}

	DX11Context::DX11Context(WindowHandle windowHandle)
		: m_WindowHandle(windowHandle)
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

		std::vector<DXRef<IDXGIAdapter>> adapters = Utils::GetAdapters();

		std::vector<size_t> discreteAdapters;
		int32 intelAdapterIndex = -1;
		int32 microsoftAdapterIndex = -1;

		for (size_t i = 0; i < adapters.size(); i++)
		{
			DXGI_ADAPTER_DESC desc;
			adapters[i]->GetDesc(&desc);

			if (desc.VendorId == 0x8086)
				intelAdapterIndex = i;
			else if (desc.VendorId == 0x1414)
				microsoftAdapterIndex = i;
			else
				discreteAdapters.push_back(i);
		}

		if (!discreteAdapters.empty())
		{
			m_Adapter = adapters[discreteAdapters.front()];
		}
		else if (intelAdapterIndex != -1)
		{
			m_Adapter = adapters[intelAdapterIndex];
		}
		else if (microsoftAdapterIndex != -1)
		{
			m_Adapter = adapters[microsoftAdapterIndex];
			MessageBoxA(0, "Microsoft Basic Render Driver was chosen as graphics device", "Warning", MB_SYSTEMMODAL | MB_ICONWARNING);
		}
	}

	DX11Context::~DX11Context()
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();
	}

	bool DX11Context::Init()
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

		HWND hWnd = static_cast<HWND>(m_WindowHandle);

		return true;
	}

	void DX11Context::SwapBuffers(int32 swapInterval)
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

	}

}