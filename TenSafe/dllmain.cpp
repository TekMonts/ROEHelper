// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "stdio.h"

DWORD ReadDWORD(DWORD adr)
{
	if (IsBadReadPtr((VOID*)adr, sizeof(DWORD))) return NULL;
	DWORD res = *(DWORD*)(adr);
	return res;
}

DWORD WINAPI InitializeHook(__in LPVOID lpParameter)
{
	while (true)
	{
		if (GetAsyncKeyState(VK_F1)) {
			DWORD MainProcedureP = ReadDWORD(0x2EF12E8);
			DWORD WeaponConfigTable = ReadDWORD(MainProcedureP + 0x60C);
			DWORD Table = ReadDWORD(WeaponConfigTable + 0x40);
			int EntrySize = 0x450;
			int RecoilOff = 0x50;
			int SpreadOff = 0x2EC;
			int RapidOff = 0x34;
			float NewF = 0.0f;
			int newRapid = 5000;
			for (int i = 0; i < 17; i++) {

				DWORD weapon = Table + (i*EntrySize);

				DWORD recoilAddr = weapon + RecoilOff;
				DWORD spreadAddr = weapon + SpreadOff;

				DWORD rapidAddr = weapon + RapidOff;

				float recoil = *(float*)recoilAddr;
				float spread = *(float*)spreadAddr;
				int rapid = *(int*)rapidAddr;

				printf("Patching weapon %d, r: %.2f, s: %.2f\n", i, recoil, spread);

				*(float*)(recoilAddr) = NewF;
				*(float*)(spreadAddr) = NewF;
				*(int*)(rapidAddr) = newRapid;
			}
		}
		if (GetAsyncKeyState(VK_MBUTTON)) {
			DWORD QSCamera = ReadDWORD(0x29E594C);
			DWORD Zoom = ReadDWORD(QSCamera + 0x4);
			DWORD ZoomDetail = ReadDWORD(Zoom + 0x10);
		}
	}
	Sleep(200);
	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule); //disable unwanted thread notifications to reduce overhead
		CreateThread(0, 0, InitializeHook, 0, 0, 0); //init our hooks

		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
    }
    return true;
}