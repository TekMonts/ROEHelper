// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

char dlldir[320];
char *GetDirectoryFile(const char *filename)
{
	static char path[320];
	strcpy_s(path, dlldir);
	strcat_s(path, filename);
	return path;
}

void Log(const char *fmt, ...)
{
	if (!fmt)	return;

	char		text[4096];
	va_list		ap;
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

	ofstream logfile(GetDirectoryFile("log.txt"), ios::app);
	if (logfile.is_open() && text)	logfile << text << endl;
	logfile.close();
}




DWORD ReadDWORD(DWORD adr)
{
	if (IsBadReadPtr((VOID*)adr, sizeof(DWORD))) return NULL;
	DWORD res = *(DWORD*)(adr);
	return res;
}

int ZoomLevel;
int MuzzleVelocity[20];
bool OnMuzzleVelocityHack, init;
float newnX, newX, newY, newnY;


DWORD WINAPI InitializeHook(__in LPVOID lpParameter)
{
	ZoomLevel = 0;
	OnMuzzleVelocityHack = false;
	init = false;
	while (1)
	{
		
		if (GetAsyncKeyState(VK_F1)) {
			if (!init) {
				Log("F1 Pressed, activating no recoil, no spread and initialize the MuzzleVelocity...\n");
				//Hack Spread and Recoil
				DWORD MainProcedureP = ReadDWORD(0x2EF12E8);
				DWORD WeaponConfigTable = ReadDWORD(MainProcedureP + 0x60C);
				DWORD Table = ReadDWORD(WeaponConfigTable + 0x40);
				int EntrySize = 0x450;
				int RecoilOff = 0x50;
				int SpreadOff = 0x2EC;
				int RapidOff = 0x34;
				float NewF = 0.0f;
				for (int i = 0; i < 20; i++) {

					DWORD weapon = Table + (i*EntrySize);

					DWORD recoilAddr = weapon + RecoilOff;
					DWORD spreadAddr = weapon + SpreadOff;
					DWORD rapidAddr = weapon + RapidOff;

					float recoil = *(float*)recoilAddr;
					float spread = *(float*)spreadAddr;
					int rapid = *(int*)rapidAddr;
					MuzzleVelocity[i] = rapid;
					Log("Muzzle Velocity default value: %d \tMuzzleVelocity[%d]: %d\n", rapid, i+1, MuzzleVelocity[i]);
					Log("Patching weapon %d, recoil: %.2f, spread: %.2f\n", i, recoil, spread);
					*(float*)(recoilAddr) = NewF;
					*(float*)(spreadAddr) = NewF;
				}
			}
			init = true;
		}

		if (GetAsyncKeyState(VK_F2)) {
		//Hack MuzzleVelocity
			OnMuzzleVelocityHack = !OnMuzzleVelocityHack;
				Log("F2 Pressed, Muzzle Velocity Hack now is %s\n", OnMuzzleVelocityHack ? "ON" : "OFF");
				int NewMuzzleVelocityValue = 0;
				DWORD MainProcedureP = ReadDWORD(0x2EF12E8);
				DWORD WeaponConfigTable = ReadDWORD(MainProcedureP + 0x60C);
				DWORD Table = ReadDWORD(WeaponConfigTable + 0x40);
				int EntrySize = 0x450;
				int MuzzleVelocityOff = 0x34;
				for (int i = 0; i < 20; i++) {
					DWORD weapon = Table + (i*EntrySize);

					DWORD MuzzleVelocityAddr = weapon + MuzzleVelocityOff;

					int MuzzleVelocityValue = *(int*)MuzzleVelocityAddr;

					if (OnMuzzleVelocityHack) {
						NewMuzzleVelocityValue = 2000;
					}
					else {
						NewMuzzleVelocityValue = MuzzleVelocity[i];
					}
					Log("MuzzleVelocity[%d]: %d, Muzzle Velocity Value to write: %d\n", i, MuzzleVelocity[i], NewMuzzleVelocityValue);
					*(int*)(MuzzleVelocityAddr) = NewMuzzleVelocityValue;
				}
				continue;
		}
		
		if (GetAsyncKeyState(VK_MBUTTON)) {
			Log("Midle Mouse Pressed, activating Zoom Hack...\n");
			//temp unload dll
			//HINSTANCE hinstLib;

			//// Get a handle to the DLL module.
			//hinstLib = LoadLibrary(TEXT("TenSafe.dll"));    //1: load the DLL

			//											  // If the handle is valid, unload the DLL
			//if (hinstLib != NULL)
			//{
			//	FreeLibrary(hinstLib);      //2: unload the DLL
			//}


			//ZoomLevel Hack: None -> X2 -> X4 -> X8
			ZoomLevel++;
			if (ZoomLevel > 3) {
				ZoomLevel = 0;
			}

			//Default ZoomLevel
			

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
				newnX = -0.857f;
				newX = 0.857f;
				newY = 0.453f;
				newnY = -0.453f;
				break;
			}

			DWORD QSCamera = ReadDWORD(0x29E594C);
			DWORD Zoom = ReadDWORD(QSCamera + 0x4);
			DWORD ZoomDetail = ReadDWORD(Zoom + 0x10);
			int X = 0x8C;
			int nX = 0x88;
			int nY = 0x94;
			int Y = 0x90;

			DWORD nXAddr = ReadDWORD(ZoomDetail + nX);
			DWORD XAddr = ReadDWORD(ZoomDetail + X);
			DWORD YAddr = ReadDWORD(ZoomDetail + Y);
			DWORD nYAddr = ReadDWORD(ZoomDetail + nY);

			float ZoomNX = *(float*)nXAddr;
			float ZoomX = *(float*)XAddr;
			float ZoomY = *(float*)YAddr;
			float ZoomNY = *(float*)nYAddr;
			Log("Current Zoom Hack level: %d, -x: %.3f, x: %.3f, -y: %.3f, y: %.3f\n", ZoomLevel, ZoomNX, ZoomX, ZoomNY, ZoomY);
			Log("Value to be apply Zoom Hack: -x: %.3f, x: %.3f, -y: %.3f, y: %.3f\n", newnX, newX, newnY, newY);
			//*(float*)(XAddr) = newX;
			Sleep(100);
			//*(float*)(nXAddr) = newnX;
			Sleep(100);
			//*(float*)(YAddr) = newY;
			Sleep(100);
			//*(float*)(nYAddr) = newnY;

		}
		Sleep(200);
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