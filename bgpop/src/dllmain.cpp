#include <windows.h>
#include <mhook/mhook-lib/mhook.h>

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		OutputDebugStringA("[bgpop] DLL_PROCESS_ATTACH");
		break;
	}
	case DLL_PROCESS_DETACH: {
		OutputDebugStringA("[bgpop] DLL_PROCESS_DETACH");
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}
	return TRUE;
}

