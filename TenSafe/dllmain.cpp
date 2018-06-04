// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

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
		int ZoomLevel = 0;
		int MuzzleVelocity[17];
		bool Init = false;
		bool OnMuzzleVelocityHack = false;

		if (GetAsyncKeyState(VK_F1)) {
			//Hack Spread and Recoil
			if (!Init) {
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
					MuzzleVelocity[i] = rapid;
					*(float*)(recoilAddr) = NewF;
					*(float*)(spreadAddr) = NewF;
					*(int*)(rapidAddr) = newRapid;
				}
			}
			Init = true;
		}

		if (GetAsyncKeyState(VK_F2)) {
		//Hack MuzzleVelocity
			int NewMuzzleVelocityValue = 0;
			DWORD MainProcedureP = ReadDWORD(0x2EF12E8);
			DWORD WeaponConfigTable = ReadDWORD(MainProcedureP + 0x60C);
			DWORD Table = ReadDWORD(WeaponConfigTable + 0x40);
			int EntrySize = 0x450;
			int MuzzleVelocityOff = 0x34;
			for (int i = 0; i < 17; i++) {
				DWORD weapon = Table + (i*EntrySize);

				DWORD MuzzleVelocityAddr = weapon + MuzzleVelocityOff;

				int MuzzleVelocityValue = *(int*)MuzzleVelocityAddr;

				if (OnMuzzleVelocityHack) {
					NewMuzzleVelocityValue = MuzzleVelocity[i] * 50;
				}
				else {
					NewMuzzleVelocityValue = MuzzleVelocity[i];
				}

				*(int*)(MuzzleVelocityAddr) = NewMuzzleVelocityValue;
			}
			OnMuzzleVelocityHack = !OnMuzzleVelocityHack;
		}
		
		if (GetAsyncKeyState(VK_MBUTTON)) {
			//ZoomLevel Hack: None -> X2 -> X4 -> X8
			ZoomLevel++;
			if (ZoomLevel > 3) {
				ZoomLevel = 0;
			}

			//Default ZoomLevel
			float newnX = -0.857f;
			float newX = 0.857f;
			float newY = 0.453f;
			float newnY = -0.453f;

			switch (ZoomLevel)
			{
			case 1: //2X Scope
				newnX = -0.548f;
				newX = 0.548f;
				newY = 0.289f;
				newnY = -0.289f;
				break;
			case 2: //4X Scope
				newnX = -0.246f;
				newX = 0.246f;
				newY = 0.130f;
				newnY = -0.130f;
				break;
			case 3: //8X Scope
				newnX = -0.136f;
				newX = 0.136f;
				newY = 0.072f;
				newnY = -0.072f;
				break;
			default: // Red Dot /None Scope
				break;
			}

			DWORD QSCamera = ReadDWORD(0x29E594C);
			DWORD Zoom = ReadDWORD(QSCamera + 0x4);
			DWORD ZoomDetail = ReadDWORD(Zoom + 0x10);
			int X = 0x8C;
			int nX = 0x88;
			int nY = 0x94;
			int Y = 0x90;

			DWORD XAddr = ReadDWORD(ZoomDetail + X);
			DWORD nXAddr = ReadDWORD(ZoomDetail + nX);
			DWORD YAddr = ReadDWORD(ZoomDetail + Y);
			DWORD nYAddr = ReadDWORD(ZoomDetail + nY);

			float ZoomX = *(float*)XAddr;
			float ZoomNX = *(float*)nXAddr;
			float ZoomY = *(float*)YAddr;
			float ZoomNY = *(float*)nYAddr;

			*(float*)(XAddr) = newX;
			*(float*)(nXAddr) = newnX;
			*(float*)(YAddr) = newY;
			*(float*)(nYAddr) = newnY;

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