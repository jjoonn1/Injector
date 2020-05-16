#include <iostream>
#include <Windows.h>
#include <memory>
#include "Injector.h"

using namespace std;

int main() {
	unique_ptr<string> dllPath = make_unique<string>("C:\\code\\MessageBoxDLL\\MessageBox\\x64\\Release\\MessageBox.dll");
	cout << "Enter pid: ";
	int pid;
	cin >> pid;

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	printMessage(processHandle, string("OpenProcess"));

	HANDLE pDllPath = VirtualAllocEx(processHandle, NULL, strlen((*dllPath.get()).c_str()) + 1, MEM_COMMIT, PAGE_READWRITE);
	printMessage(pDllPath, string("VirtualAllocEx"));

	BOOL result = WriteProcessMemory(processHandle, pDllPath, (LPVOID)(dllPath.get()), strlen((*dllPath.get()).c_str()) + 1, 0);
	printMessage(HANDLE(result), string("WriteProcessMemory"));

	HANDLE remoteThread = CreateRemoteThread(processHandle, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32.dll"), "LoadLibraryA"), pDllPath, 0, NULL);
	printMessage(remoteThread, string("CreateRemoteThread"));

	WaitForSingleObject(remoteThread, INFINITE);
}

string getErrorMessage(int errorNumber) {
	LPSTR errorMessage = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorNumber, NULL, (LPSTR)&errorMessage, 0, NULL);
	return string(errorMessage);
}

void printMessage(HANDLE result,string &functionName) {
	if (!result) {
		auto lastError = GetLastError();
		string errorMessage = string(getErrorMessage(lastError));
		cout << "An error occured in " << functionName << ", error number: " << lastError << ", " << errorMessage;
	}
}