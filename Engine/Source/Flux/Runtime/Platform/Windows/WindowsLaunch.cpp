#include "FluxPCH.h"

#ifdef FLUX_PLATFORM_WINDOWS

namespace Flux {

	extern int32 Main();
	
	HINSTANCE g_Instance;

	int32 Launch(HINSTANCE hInstance)
	{
		g_Instance = hInstance;
		return Main();
	}

}

#ifdef FLUX_BUILD_SHIPPING
	#ifdef _UNICODE
		int32 WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int32 nShowCmd)
		{
			return Flux::Launch(hInstance);
		}
	#else
		int32 WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int32 nShowCmd)
		{
			return Flux::Launch(hInstance);
		}
	#endif
#else
	int32 main(int32 argc, char* argv[])
	{
		return Flux::Launch(GetModuleHandle(NULL));
	}
#endif

#endif
