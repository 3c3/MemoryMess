#include <iostream>
#include <Windows.h>

using namespace std;

struct DeleterData
{
	void* funcAddr;
	char* szFilename;
};

struct DeleterTwoData
{
	void* loadLibraryFunc; // +0
	void* getProcAddrFunc; // +4
	char* kernelName;      // +8
	char* sleepName;       // +12
	char* deleteFileName;  // +16
	char* szFilename;      // +20
};

typedef int (__stdcall *DeleterPrototype)(DeleterData *data);
typedef int(__stdcall *DeleterTwoPrototype)(DeleterTwoData *data);

char deleterBytecode[] = { 0x55, 0x89, 0xE5, 0x83, 0xEC, 0x04, 0x8B, 0x55, 0x08, 0xFF, 0x72, 0x08, 0xFF, 0x12, 0x89, 0x45, 0xFC, 0x8B, 0x55, 0x08, 0xFF, 0x72, 0x0C, 0xFF, 0x75, 0xFC, 0xFF, 0x52, 0x04, 0x68, 0xDC, 0x05, 0x00, 0x00, 0xFF, 0xD0, 0x8B, 0x55, 0x08, 0xFF, 0x72, 0x10, 0xFF, 0x75, 0xFC, 0xFF, 0x52, 0x04, 0x8B, 0x55, 0x08, 0xFF, 0x72, 0x14, 0xFF, 0xD0, 0x89, 0xEC, 0x5D, 0xC2, 0x04, 0x00 };
int main()
{
	/*char buffer[256];

	if (GetModuleFileName(0, buffer, 256))
	{
		cout << "module: " << buffer << endl;
	}
	else
	{
		int error = GetLastError();
		cout << "error: " << error << endl;
	}*/

	DeleterTwoPrototype function = (DeleterTwoPrototype)VirtualAlloc(0, sizeof(deleterBytecode), 
										MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(function, deleterBytecode, sizeof(deleterBytecode));
	//FlushInstructionCache(GetCurrentProcess(), 0, 0);

	char deleteFileName[] = "DeleteFileA\0";
	char sleepName[] = "Sleep\0";
	char kernelName[] = "kernel32.dll\0";

	DeleterTwoData *dtw = new DeleterTwoData;
	dtw->loadLibraryFunc = LoadLibrary;
	dtw->getProcAddrFunc = GetProcAddress;

	dtw->deleteFileName = new char[sizeof(deleteFileName)];
	memcpy(dtw->deleteFileName, deleteFileName, sizeof(deleteFileName));

	dtw->sleepName = new char[sizeof(sleepName)];
	memcpy(dtw->sleepName, sleepName, sizeof(sleepName));

	dtw->kernelName = new char[sizeof(kernelName)];
	memcpy(dtw->kernelName, kernelName, sizeof(kernelName));

	dtw->szFilename = new char[128];
	cin >> dtw->szFilename;

	HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)function, dtw, 0, 0);
	WaitForSingleObject(hThread, INFINITE);

	DWORD exitCode;
	int result = GetExitCodeThread(hThread, &exitCode);

	cout << result << " " << exitCode << endl;

	system("pause");
}