#include <iostream>
#include <Windows.h>
#include "Injector.h"

using namespace std;

int main() {
	LPCSTR dllPath = "C:\\code\\MessageBox\\x64\\Release\\MessageBox.dll";
	cout << "Enter pid: ";
	int pid;
	cin >> pid;
	auto processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!processHandle) {
		auto lastError = GetLastError();
		auto errorMessage = getErrorMessage(lastError);
		cout << "An error occured in OpenProcess, error number: " << lastError << " " << errorMessage;
		return 1;
	}
	LPVOID pDllPath = VirtualAllocEx(processHandle, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (!pDllPath) {
		auto lastError = GetLastError();
		auto errorMessage = getErrorMessage(lastError);
		cout << "An error occured in VirtualAlloc, error number: " << lastError << " " << errorMessage;
		return 1;
	}
	auto result = WriteProcessMemory(processHandle, pDllPath, (LPVOID)dllPath, strlen(dllPath) + 1, 0);
	if (!result) {
		auto lastError = GetLastError();
		auto errorMessage = getErrorMessage(lastError);
		cout << "An error occured in WriteProcessMemory, error number: " << lastError << " " << errorMessage;
		return 1;
	}
	auto remoteThread = CreateRemoteThread(processHandle, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32.dll"), "LoadLibraryA"), pDllPath, 0, NULL);
	if (!remoteThread) {
		auto lastError = GetLastError();
		auto errorMessage = getErrorMessage(lastError);
		cout << "An error occured in CreateRemoteThread, error number: " << lastError << " " << errorMessage;
		return 1;
	}
	WaitForSingleObject(remoteThread, INFINITE);
}

std::string getErrorMessage(int errorNumber) {
	LPSTR errorMessage = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorNumber, NULL, (LPSTR)&errorMessage, 0, NULL);
	return std::string(errorMessage);
}