#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11_3.h>
#include <d3d11_4.h>

#include <dxgi.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>

#include <wrl/client.h>
#include <comdef.h>

namespace Flux {

	template<typename T>
	using DXRef = Microsoft::WRL::ComPtr<T>;

#define DX_RIID(i) __uuidof(i)

#ifdef FLUX_BUILD_DEBUG
	#define DX_CHECKS
#endif

#ifdef DX_CHECKS
	#define DX_CHECK(hr) \
	{ \
		if (!SUCCEEDED(hr)) \
		{ \
			_com_error error(hr); \
			__debugbreak(); \
		} \
	}
#else
	#define DX_CHECK(hr) hr
#endif


}