#pragma once
#include <windows.h>

#include <stdio.h>
#include <fstream>
#include <stdint.h>
using namespace std;

LRESULT __declspec(dllexport)__stdcall  CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
extern "C" BOOL __declspec(dllexport) InstallHook();
extern "C" BOOL __declspec(dllexport)  UnHook();
DWORD ReadDWORD(DWORD adr);
char *GetDirectoryFile(const char *filename);
void Log(const char *fmt, ...);
void NoRecoilMuzzleVelocity();
void MuzzleVelocityHack();
void CameraZoomHack();
float* getZoomValue(int level);
DWORD WINAPI InitThread(LPVOID lpParam);
