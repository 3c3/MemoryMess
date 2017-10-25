// Полезни неща за инжектиране

#pragma once
#include <Windows.h>

/// Създава нов Notepad процес с нужните привилегии
HANDLE MakeProcess(bool suspended = false);

/// Копира функция и параметър, след това изпълнява
/// Връща резултата от функцията
int RunRemoteFunction(HANDLE process, void* function, int functionSize, void* data, int dataSize, bool wait);

/// Копира низ в целевия процес
/// Връща pointer към низа в него
void* InjectString(HANDLE process, char* str);