#include "helper.h"

char dlldir[320];
int ZoomLevel = 0;
int MuzzleVelocity[20];
bool OnMuzzleVelocityHack = false, init = false;

DWORD ReadDWORD(DWORD adr)
{
	if (IsBadReadPtr((VOID*)adr, sizeof(DWORD))) return NULL;
	DWORD res = *(DWORD*)(adr);
	return res;
}

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

void NoRecoilMuzzleVelocity()
{
	if (!init) {
		Log("F3 Pressed, activating no recoil, no spread and initialize the MuzzleVelocity...\n");
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
			Log("Muzzle Velocity default value: %d \tMuzzleVelocity[%d]: %d\n", rapid, i + 1, MuzzleVelocity[i]);
			Log("Patching weapon %d, recoil: %.2f, spread: %.2f\n", i, recoil, spread);
			*(float*)(recoilAddr) = NewF;
			*(float*)(spreadAddr) = NewF;
		}
	}
	else
	{
		Log("F3 Pressed, Already activated no recoil, no spread and inited MuzzleVelocity! Do nothing...");
	}

	init = true;

}

void MuzzleVelocityHack()
{
	//Hack MuzzleVelocity
	OnMuzzleVelocityHack = !OnMuzzleVelocityHack;
	Log("F4 Pressed, Muzzle Velocity Hack now is %s\n", OnMuzzleVelocityHack ? "ON" : "OFF");
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
}


float* getZoomValue(int level) {

	if (level == 1)
	{
		return new float[2]{ 0.548f, 0.289f };
	}
	else if (level == 2)
	{
		return new float[2]{ 0.246f, 0.130f };
	}
	else if (level == 3)
	{
		return new float[2]{ 0.136f, 0.072f };
	}

	else return new float[2]{ 0.857f, 0.453f };
}


void CameraZoomHack()
{
	float* ZoomValue;
	Log("Midle Mouse Pressed, activating Zoom Hack...\n");

	ZoomLevel++;
	if (ZoomLevel > 3) {
		ZoomLevel = 0;
	}

	ZoomValue = getZoomValue(ZoomLevel);

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
	Log("Value to be apply Zoom Hack: -x: %.3f, x: %.3f, -y: %.3f, y: %.3f\n", -ZoomValue[0], ZoomValue[0], -ZoomValue[1], ZoomValue[1]);
	
	*(float*)(XAddr) = ZoomValue[0];
	*(float*)(nXAddr) = -ZoomValue[0];
	*(float*)(YAddr) = ZoomValue[1];
	*(float*)(nYAddr) = -ZoomValue[1];
}

DWORD WINAPI InitThread(LPVOID lpParam)
{
	InstallHook();
	return 0;
}