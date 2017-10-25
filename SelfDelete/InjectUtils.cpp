#include "InjectUtils.h"

HANDLE MakeProcess(bool suspended)
{
	char notepadDir[256];
	GetWindowsDirectory(notepadDir, 256);
	char appString[] = "\\System32\\notepad.exe\0";
	strcat_s(notepadDir, appString);

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	BOOL ok = CreateProcess(notepadDir, 0, 0, 0, 0,
		suspended ? CREATE_SUSPENDED : 0, 0, 0,
		&si, &pi);

	if (ok) return pi.hProcess;
	else return 0;
}

int RunRemoteFunction(HANDLE process, void* function, int functionSize, void* data, int dataSize, bool wait)
{
	DWORD retVal;

	// Проверките за грешки малко ги няма
	void* functionAddr = VirtualAllocEx(process, 0, functionSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(process, functionAddr, function, functionSize, &retVal);
	VirtualProtectEx(process, functionAddr, functionSize, PAGE_EXECUTE_READ, &retVal);

	void* dataAddr = VirtualAllocEx(process, 0, functionSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(process, dataAddr, data, dataSize, &retVal);

	HANDLE thread = CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)functionAddr, dataAddr, 0, 0);

	if (wait)
	{
		if (thread != INVALID_HANDLE_VALUE)
		{
			WaitForSingleObject(thread, INFINITE);
			GetExitCodeThread(thread, &retVal);
			return retVal;
		}

		return -1;
	}
	else return 0;
}

void* InjectString(HANDLE process, char* str)
{
	int len = strnlen_s(str, 1024);

	void* strAddr = VirtualAllocEx(process, 0, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(process, strAddr, str, len, 0);

	return strAddr;
}