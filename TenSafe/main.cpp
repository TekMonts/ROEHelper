// dllmain.cpp : Defines the entry point for the DLL application.
#include "helper.h"
HMODULE hInstance = 0;
HHOOK hkb;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInstance = hModule;
		DisableThreadLibraryCalls(hModule); //disable unwanted thread notifications to reduce overhead
		CreateThread(0, 0, InitThread, 0, 0, 0); //init our hooks

		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}



LRESULT __declspec(dllexport)__stdcall  CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	while (true) {
		if (((DWORD)lParam & 0x40000000) && (HC_ACTION == nCode))
		{
			if (wParam == VK_F3)
			{
				NoRecoilMuzzleVelocity();
				Sleep(200);
			};

			if (wParam == VK_F4)
			{
				MuzzleVelocityHack();
				Sleep(200);
			};

			if (wParam == VK_F5)
			{
				CameraZoomHack();
				Sleep(200);
			};
			Sleep(200);
		}

		Sleep(200);
		return CallNextHookEx(hkb, nCode, wParam, lParam);
	}
}


BOOL InstallHook()
{
	hkb = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, hInstance, 0);

	return TRUE;
}

BOOL UnHook()
{
	BOOL unhooked = UnhookWindowsHookEx(hkb);
	return unhooked;
}