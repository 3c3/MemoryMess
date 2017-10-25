#include <iostream>
#include <windows.h>

using namespace std;

typedef int (__stdcall *testfunction)();
typedef int(__stdcall *testfunctionwitharg)(int);
typedef int(__stdcall *arg2func)(int, int);

testfunction f1;

int emptyFunc(int k)
{
	k ^= 13;
	return k+1;
}

//char funcBytes[] = { 0x31, 0xC0, 0x40, 0xC3 };
//char funcBytes[] = { 0x59, 0x58, 0x01, 0xC0, 0x51, 0xC3 };
char funcBytes[] = { 0x55, 0x89, 0xE5, 0x8B, 0x45, 0x08, 0xF7, 0x6D, 0x0C, 0x89, 0xEC, 0x5D, 0xC2, 0x08, 0x00 };

int main()
{
	void* funcAddr = VirtualAlloc(0, sizeof(funcBytes), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	memcpy(funcAddr, funcBytes, sizeof(funcBytes));

	DWORD oldProtect;

	if (!VirtualProtect(funcAddr, sizeof(funcBytes), PAGE_EXECUTE, &oldProtect)) cout << "VProtect failed\n";
	if (!FlushInstructionCache(GetCurrentProcess(), 0, 0)) cout << "Flush failed\n";

	arg2func func = (arg2func)funcAddr;

	int result = func(10, 5);

	cout << result << endl;
	system("pause");
}
