// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "includes.h"

HWND hwnd;

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
	DWORD dwPid = 0;
	GetWindowThreadProcessId(hWnd, &dwPid);
	if (dwPid == lParam)
	{
		hwnd = hWnd;
		return FALSE;
	}
	return TRUE;
}

HWND get_process_window()
{
	if (hwnd)
		return hwnd;

	EnumWindows(EnumWindowCallBack, GetCurrentProcessId());

	if (hwnd == NULL)
		0;
	return hwnd;
}



BOOL APIENTRY DllMain(HMODULE hmodule, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		ctx::data::base = (uintptr_t)GetModuleHandleA(0);
		wndproc = get_process_window();
		utility::I_beginthreadex(0, 0, (_beginthreadex_proc_type)StartPoint::Init, 0, 0, 0);

	}

	return TRUE;
}