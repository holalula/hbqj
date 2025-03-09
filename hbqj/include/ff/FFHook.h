// TODO: refactor function types
#pragma once
#include "../../pch.h"
#include <iostream>
#include <tchar.h>
#include <TlHelp32.h>
#include <vector>
typedef void (*SETINT)(int);
typedef int (*GETINT)();
typedef __int64 (*GETINT64)();
typedef void (*SETFLOAT)(float);

class DLLAPI FFHook {
public:
	~FFHook() {  };
	FFHook() {};
	FFHook(const FFHook&) = delete;
	FFHook& operator=(const FFHook&) = delete;
	static FFHook& get_instance() {
		static FFHook instance;
		return instance;
	}

	int LoadDLLGetFunc(const wchar_t* Path);
	int InjectDLL(const wchar_t* Path, DWORD pid);

	HMODULE hModule;
	
	SETINT setInit;
	GETINT getInit;
	GETINT get_Import_c;
	GETINT get_Total_count;
	SETINT set_Import_c;
	SETINT set_Total_count;
	SETINT setMode;
	GETINT getMode;

	GETINT getStartLoad;
	SETINT setStartLoad;

	GETINT64 getOPERATE_P1;
	GETINT64 getLoadhouse_P1;

	SETINT setGuizmoFlag;
	GETINT getGuizmoFlag;
	
	SETINT setGuizmoSnapFlag;
	GETINT getGuizmoSnapFlag;
	SETFLOAT setGuizmoSnap;

	SETINT setSavepreview;

	//EX CALL
	int PreviewOnce();
	int StartImport();
	int GetImportProgress();

	int OpenGuizmo();
	int CloseGuizmo();
	int OnGuizmoSnap();
	int OffGuizmoSnap();
	int SetGuizmoSnap(float s);

	int BeginArrange();

	int SetSavePreview(int s);
};
int FFHook::LoadDLLGetFunc(const wchar_t* Path) {
	hModule = LoadLibraryW(Path);
	if (hModule != NULL) {
		std::cout << "LoadDLLGetFunc:" << std::endl;

		setInit = (SETINT)GetProcAddress(hModule,"setInit");
		getInit = (GETINT)GetProcAddress(hModule,"getInit");
		get_Import_c = (GETINT)GetProcAddress(hModule,"get_import_c");
		get_Total_count = (GETINT)GetProcAddress(hModule,"get_total_count");
		set_Import_c = (SETINT)GetProcAddress(hModule, "set_import_c");
		set_Total_count = (SETINT)GetProcAddress(hModule, "set_total_count");

		setMode = (SETINT)GetProcAddress(hModule, "setMode");
		getMode = (GETINT)GetProcAddress(hModule, "getMode");

		getStartLoad = (GETINT)GetProcAddress(hModule, "getStartLoad");
		setStartLoad = (SETINT)GetProcAddress(hModule, "setStartLoad");

		getOPERATE_P1 = (GETINT64)GetProcAddress(hModule, "getOPITEM_P1");
		getLoadhouse_P1 = (GETINT64)GetProcAddress(hModule, "getLOADHOUSE_P1");

		setGuizmoFlag = (SETINT)GetProcAddress(hModule, "setguizmoFlag");
		getGuizmoFlag = (GETINT)GetProcAddress(hModule, "getguizmoFlag");

		setGuizmoSnapFlag = (SETINT)GetProcAddress(hModule, "setguizmoSnapFlag");
		getGuizmoSnapFlag = (GETINT)GetProcAddress(hModule, "getguizmoSnapFlag");
		setGuizmoSnap = (SETFLOAT)GetProcAddress(hModule, "setguizmoSnap");

		setSavepreview = (SETINT)GetProcAddress(hModule, "setSavePreview");

		std::cout << "SucLoadDLLGetFunc:" << std::endl;
		return 1;
	}
	return 0;
}

int FFHook::InjectDLL(const wchar_t* Path, DWORD pid) {
	HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
	if (!hp) {
		std::cout << "5fail" << std::endl;
		std::cout << GetLastError() << std::endl;
		return 0;
	}
	SIZE_T PathSize = (_tcslen(Path) + 1) * sizeof(TCHAR);
	LPVOID StartAddress = VirtualAllocEx(hp, NULL, PathSize, MEM_COMMIT, PAGE_READWRITE);
	if (!StartAddress) {
		std::cout << "4fail" << std::endl;
		std::cout << GetLastError() << std::endl;
		return 0;
	}
	if (!WriteProcessMemory(hp, StartAddress, Path, PathSize, NULL)) {
		std::cout << "3fail" << std::endl;
		std::cout << GetLastError() << std::endl;
		return 0;
	}
	PTHREAD_START_ROUTINE pfnStartAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryW");
	if (!pfnStartAddress) {
		std::cout << "2fail" << std::endl;
		std::cout << GetLastError() << std::endl;
		return 0;
	}

	HANDLE hThread = CreateRemoteThreadEx(hp, NULL, NULL, pfnStartAddress, StartAddress, NULL, NULL, NULL);
	if (!hThread) {
		std::cout << "1fail" << std::endl;
		std::cout << GetLastError() << std::endl;
		std::cout << "---lasterror" << std::endl;
		return 0;
	}
	std::cout << "StartWait" << std::endl;
	WaitForSingleObject(hThread, 5000);
	CloseHandle(hThread);
	CloseHandle(hp);

	std::cout << "inject suc" << std::endl;
	return 1;
}

int FFHook::PreviewOnce() {
	setStartLoad(1);
	return 1;
}

int FFHook::StartImport() {
	setInit(1);
	return 1;
}

int FFHook::GetImportProgress() {
	int _c = get_Import_c();
	int _count = get_Total_count();
	return _c;
}

int FFHook::OpenGuizmo() {
	setGuizmoFlag(1);
	return 1;
}
int FFHook::CloseGuizmo() {
	setGuizmoFlag(0);
	return 1;
}
int FFHook::OnGuizmoSnap(){
	setGuizmoSnapFlag(1);
	return 1;
}
int FFHook::OffGuizmoSnap(){
	setGuizmoSnapFlag(0);
	return 1;
}
int FFHook::SetGuizmoSnap(float s) {
	setGuizmoSnap(s);
	return 1;
}
int FFHook::BeginArrange() {
	setInit(1);
	return 1;
}

int FFHook::SetSavePreview(int _s) {
	setSavepreview(_s);
	return 1;
}