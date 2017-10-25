// Самоизтриваща се програма с инжектиране
// Първо проработи на 25.10.2017 (100 години след октомврийската революция)

#include <iostream>
#include <Windows.h>
#include "InjectUtils.h"

using namespace std;

// Вариант, който оставя Notepad-а отворен

//struct InjectData
//{
//	void* sleepAddr;
//	void* deleteFileAddr;
//	void* szFileName;
//};
/* NASM код:
[bits 32]
mov edx, [ebp+8]

push dword 500
call [edx] ; Sleep

mov edx, [ebp+8]
push dword [edx+8]
call [edx+4] ; DeleteFile

ret 4
*/
//char bytecode[] = { 0x8B, 0x55, 0x08, 0x68, 0xF4, 0x01, 0x00, 0x00, 0xFF, 0x12, 0x8B, 0x55, 0x08, 0xFF, 0x72, 0x08, 0xFF, 0x52, 0x04, 0xC2, 0x04, 0x00 };

// ------------------------------------------
// Със затваряне на Notepad-a

struct InjectData
{
	void* sleepAddr;			// +0
	void* deleteFileAddr;		// +4
	void* szFileName;			// +8
	void* getCurrentProcAddr;	// +12
	void* terminateProcAddr;	// +16
};

/* NASM код: 
[bits 32]
mov edx, [ebp+8]

push dword 500
call [edx] ; Sleep

mov edx, [ebp+8]
push dword [edx+8]
call [edx+4] ; DeleteFile

mov edx, [ebp+8] ; restore

call [edx+12] ; GetCurrentProcess

mov edx, [ebp+8] ; restore

push dword 0
push eax
call [edx+16] ; TerminateProcess(current, 0)

ret 4
*/

char bytecode[] = { 0x8B, 0x55, 0x08, 0x68, 0xF4, 0x01, 0x00, 0x00, 0xFF, 0x12, 0x8B, 0x55, 0x08, 0xFF, 0x72, 0x08, 0xFF, 0x52, 0x04, 0x8B, 0x55, 0x08, 0xFF, 0x52, 0x0C, 0x8B, 0x55, 0x08, 0x6A, 0x00, 0x50, 0xFF, 0x52, 0x10, 0xC2, 0x04, 0x00 };

int main()
{
	MessageBox(0, "Goodbye cruel world", "Final message", MB_OK);

	char exeFileName[256];

	if (GetModuleFileName(0, exeFileName, 256))
	{
		cout << "module: " << exeFileName << endl;
	}
	else
	{
		int error = GetLastError();
		cout << "error: " << error << endl;
	}

	InjectData iData;
	iData.sleepAddr = Sleep;
	iData.deleteFileAddr = DeleteFileA;
	iData.getCurrentProcAddr = GetCurrentProcess;
	iData.terminateProcAddr = TerminateProcess;

	HANDLE proc = MakeProcess(true); // Процеса работник (суспендиран Notepad)

	iData.szFileName = InjectString(proc, exeFileName);

	int retVal = RunRemoteFunction(proc, bytecode, sizeof(bytecode), &iData, sizeof(iData), false);

	return 0;
}