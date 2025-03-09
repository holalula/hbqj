#ifndef __FFPROCESS_H
#define __FFPROCESS_H
#pragma once
#include "../../pch.h"
#include "../utils/md5.h"
#include "../utils/sigscanner.h"
#include "../utils/strutils.h"
#include "../utils/device.h"
#include "../utils/memory.h"
#include "../model/housing.h"
#include "../model/rotation.h"
#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/filereadstream.h"
#include "../rapidjson/filewritestream.h"
#include "../rapidjson/writer.h"
#include "OffsetMgr.h"
#include "FFHook.h"
#include "FFLog.h"
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <Shlobj.h>
#include <io.h>
#include <direct.h>

using namespace Rotation;
using namespace Memory;
using namespace FFLog;
using FFLog::Log;
using FFLog::int_to_hex;

#define MAX_PATH 512


class DLLAPI FFProcess {
public:
	~FFProcess() {};
	FFProcess() { 
		Log() << "CREATE FFPROCESS" << std::endl;
		LogSystemInfo();
		window = FindWindow(L"FFXIVGAME", NULL);
		Log() << "FindWindow: " + int_to_hex(window) << std::endl;
		this->OpenFFXIVGetBaseAdd();
	};
	FFProcess(const FFProcess&) = delete;
	FFProcess& operator=(const FFProcess&) = delete;
	static FFProcess& get_instance() {
		static FFProcess instance;
		return instance;
	}


	DWORD ProcessFind(LPCTSTR Exename);
	int OpenProcessByPid();
	int GetBaseAddressByPid();
	int OpenFFXIVGetBaseAdd();
	int UpdateOffset();
	SIZE_T GetBaseAdd();

	//MemoryPosRotation_1
	std::vector<float> GetActiveItemPos();
	int SetActiveItemX(float x);
	int SetActiveItemY(float y);
	int SetActiveItemZ(float z);
	float GetActiveItemRotation();
	int SetActiveItemRotation(float r);

	//IO
	std::string GetLocalAppDataPath();
	int LocalAppDataExist();
	int write_catelist = 0;
	int CateList2JsonFileTemp();
	std::vector<CategoryInstance> ReadTempCateList();
	std::vector<CategoryInstance> Boogiepop_Import_ReadCateList();
	std::vector<CategoryInstance> Boogiepop_Preview_ReadCateList();

	//ImportExport
	std::vector<FurnitureInstance> vfmemory;
	std::vector<CategoryInstance> vcategory;
	int catex[4096] = { -1 };
	int GetFurnitureListFromMemory();
	int FurList2CateList();
	int CateList2Json(const char* FilePath);
	int ExportCateJsonTempFile();
	std::vector<CategoryInstance> ReadCateListFromJson(const char* FilePath);
	int Boogiepop_GetFurnitureListFromMemory(std::vector<FurnitureInstance> &vf, __int64 loadhouseP1);

	int FP_Boogiepop_BeginArranging();
	int FP_Boogiepop_GetImportProgress();

	//Inject
	int Load_Inject(const wchar_t* dllpath);
	int FP_Boogiepop_Preview();
	void LoadInjectBoogiepop();

	//PlaceAnywhere
	int InjectPlaceAnywhere();
	int reInjectPlaceAnywhere();

	//Guizmo
	bool CanEdit();
	bool isRotating();
	bool IsHousing();
	std::vector<float> GetSelectedItemPos();
	std::vector<float> GetSelectedItemRotation();
	void SetSelectedItemPos(std::vector<float>);
	void SetPos2X(float x);
	void SetPos2Y(float y);
	void SetPos2Z(float z);
	void SetSelectedItemRotation(std::vector<float>);
	int FP_Boogiepop_OpenGuizmo();
	int FP_Boogiepop_CloseGuizmo();
	int FP_Boogiepop_OpenGuizmoSnap();
	int FP_Boogiepop_CloseGuizmoSnap();
	int FP_Boogiepop_SetGuizmoSnap(float s);

	//SavePreview
	int FP_Boogiepop_OnSavePreview();
	int FP_Boogiepop_OffSavePreview();

	//Time
	void SetTime(unsigned int time);
	void DisableSetTime();
	
	//Actortable
	std::vector<char> GetActorName();
	int GetActorHomeworld();
	int GetActorId();

	// New import method
	void import();

	DWORD pid = 0;
	SIZE_T baseAdd = 0;
	HWND window;
	HANDLE hProcess = NULL;
	DWORD lastError = 0;

	SIZE_T dxgi_present_offset = 0;
	SIZE_T get_dxgi_present();

	int total_count = 0, import_count = 0;
	void set_rotate_mode() {
		WriteGameMemory<int>(this->hProcess, baseAdd, 2, OffsetMgr::ActiveItemState2_moveORrotate);
	}
	typedef void(__fastcall* _select)(SIZE_T housing, SIZE_T item);
	_select my_select;
	SIZE_T housing;
	void init_my_select() {
		my_select = (_select)(GetBaseAdd() + OffsetMgr::offset_Select);
		housing = ReadGameMemory<SIZE_T>(this->hProcess, baseAdd, { OffsetMgr::layoutWorld , 0x40 });
	}
	void set_rotation_radians(float radians) {
		std::vector<SIZE_T> Qua_y{ OffsetMgr::baseHouse,0x40,0x18,0x60 + 0x4 };
		std::vector<SIZE_T> Qua_w{ OffsetMgr::baseHouse,0x40,0x18,0x60 + 0x4 + 0x8 };
		auto qua = Euler2Qua({ 0, radians * Rad2Deg,0 });
		WriteGameMemory<float>(this->hProcess, baseAdd, qua.y, Qua_y);
		WriteGameMemory<float>(this->hProcess, baseAdd, qua.w, Qua_w);
	}
	vector<unsigned long long> get_furniture_addr_list() {
		FurnitureList furniture_list = ReadGameMemory<FurnitureList>(this->hProcess, baseAdd, OffsetMgr::Furniture_List);
		vector<unsigned long long> ret;
		for (int i = 0; i < 400; i++) {
			if (furniture_list.objs[i] != 0) {
				ret.emplace_back(furniture_list.objs[i]);
			}
		}
		return ret;
	}
	struct Nop9 {
		char op[9] = { (char)0x90, (char)0x90 ,(char)0x90 ,(char)0x90 ,(char)0x90 ,(char)0x90 ,(char)0x90 ,(char)0x90 ,(char)0x90 };
	};
	void set_select_nop() {
		Nop9 nop9;
		SIZE_T dwSize;
		WriteProcessMemory(this->hProcess, (LPVOID)(baseAdd + OffsetMgr::offset_Select), &nop9, sizeof(nop9), &dwSize);
	}
};

SIZE_T FFProcess::get_dxgi_present() {
	return dxgi_present_offset;
}

#pragma region OpenProcess
DWORD FFProcess::ProcessFind(LPCTSTR Exename) {
	HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (!hp) {
		Log(LogType::error) << "ProcessFind - CreateToolhelp32Snapshot failed - Errnum: "  + std::to_string(GetLastError()) << std::endl;
		return false;
	}
	PROCESSENTRY32 info;
	info.dwSize = sizeof(info);
	if (!Process32First(hp, &info)) {
		Log(LogType::error) << "ProcessFind - Process32First failed - Errnum: "  + std::to_string(GetLastError()) << std::endl;
		return false;
	}
		
	while (1) {
		if (_tcscmp(info.szExeFile, Exename) == 0) {
			this->pid = info.th32ProcessID; //pid
			Log() << "ProcessFind pid: " + std::to_string(this->pid) << std::endl;
			return info.th32ProcessID;
		}
		if (!Process32Next(hp, &info)) {
			Log(LogType::error) << "ProcessFind - Process32Next failed - Errnum: "  + std::to_string(GetLastError()) << std::endl;
			return false;
		}
	}
	Log(LogType::error) << "ProcessFind - failed - Errnum: "  + std::to_string(GetLastError()) << std::endl;
	return false;
}

int FFProcess::OpenProcessByPid() {
	this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, this->pid);
	if (this->hProcess == NULL) {
		Log(LogType::error) << "OpenProcessByPid - OpenProcess failed - Errnum: "  + std::to_string(GetLastError()) << std::endl;
		return -1;
	}
	Log() << "OpenProcessByPid Succeeded" << std::endl;
	return 0;
}

int FFProcess::GetBaseAddressByPid() {
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->pid);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		Log(LogType::error) << "GetBaseAddressByPid - CreateToolhelp32Snapshot failed - Errnum: "  + std::to_string(GetLastError()) << std::endl;
		return -1;
	}
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);
		Log(LogType::error) << "GetBaseAddressByPid - Module32First failed - Errnum: "  + std::to_string(GetLastError()) << std::endl;
		return -1;
	}
	this->baseAdd = (SIZE_T)me32.modBaseAddr;
	CloseHandle(hModuleSnap);
	Log() << "GetBaseAddressByPid Succeeded" << std::endl;
	return 0;
}

int FFProcess::UpdateOffset() {
	LPCSTR PA1_Signature = "\xC6\x87\x73\x01\x00\x00\x00\x4C\x8B";
	LPCSTR PA1_Mask = "xxxxxx?xx";

	LPCSTR PA2_Signature = "\xC6\x87\x73\x01\x00\x00\x00\x80\xBF\x73\x01\x00\x00";
	LPCSTR PA2_Mask = "xxxxxx?xxxxxx";

	LPCSTR PA3_Signature = "\xC6\x87\x73\x01\x00\x00\x00\x48\x81\xC4";
	LPCSTR PA3_Mask = "xxxxxx?xxx";

	LPCSTR BLEU_Signature = "\x48\x85\x00\x0F\x84\x00\x00\x00\x00\x48\x89\x00\x00\x00\x57\x48\x83\xEC\x00\x48\x8B\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x33\xD2";
	LPCSTR BLEU_Mask = "xx?xx????xx???xxxx?xx?xx?x????xx";

	LPCSTR OperateItem_Signature = "\x57\x48\x83\xEC\x00\x48\x8B\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x8B\x07";
	LPCSTR OperateItem_Mask = "xxxx?xx?xx?x????xx";

	LPCSTR Loadhouse_Signature = "\x85\xC0\x74\x00\x48\x8D\x00\x00\xE9\x00\x00\x00\x00\x48\x8B";
	LPCSTR Loadhouse_Mask = "xxx?xx??x????xx";

	LPCSTR Viewmatrix_Signature = "\xE8\x00\x00\x00\x00\x48\x8D\x4C\x24\x00\x48\x89\x4C\x24\x00\x4C\x8D\x4D\x00\x4C\x8D\x44\x24\x00";
	LPCSTR Viewmatrix_Mask = "x????xxxx?xxxx?xxx?xxxx?";

	LPCSTR Basehouse_Signature = "\x48\x8B\x00\x00\x00\x00\x00\x48\x85\x00\x74\x00\x48\x8B\x00\x00\xE9\x00\x00\x00\x00\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	LPCSTR Basehouse_Mask = "xx?????xx?x?xx??x????xxxxxxxxxxxx";

	LPCSTR SavePreview_Signature = "\x48\x89\x00\x00\x00\x48\x89\x00\x00\x00\x55\x48\x8D\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33\x00\x48\x89\x00\x00\x0F\x10";
	LPCSTR SavePreview_Mask = "xx???xx???xxx???xxx????xx?????xx?xx??xx";

	LPCSTR select_signature = "\xE8\x00\x00\x00\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x48\x8B\x00\xE8";
	LPCSTR select_mask = "x????xx?x????xx???xx?x";

	LPCSTR layoutworld_signature = "\x48\x8B\x00\x00\x00\x00\x00\x48\x85\x00\x74\x00\x48\x8B\x00\x00\xE9\x00\x00\x00\x00\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC";
	LPCSTR layoutworld_mask = "xx?????xx?x?xx??x????xxxxxxxxxxxx";

	LPCSTR actortable_signature = "\x48\x8D\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x44\x0F\x00\x00\x00\x00\x00\x00\xC6\x83\x78\x3B\x00\x00";
	LPCSTR actortable_mask = "xx?????x????xx??????xxxxxx";

	LPCSTR housingmodule_signature = "\x48\x39\x00\x00\x00\x00\x00\x75\x00\x45\x33\x00\x33\xD2\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x85\x00\x74\x00\x48\x8B\x00\xE8\x00\x00\x00\x00\x48\x89";
	LPCSTR housingmodule_mask = "xx?????x?xx?xxx????x????xx?x?xx?x????xx";

	LPCSTR Present_Signature = "\x41\x8B\xF0\x8B\xFA\x89\x54\x24\x00\x48\x8B\xD9\x48\x89\x4D\x00\xC6\x44\x24\x00\x00";
	LPCSTR Present_Mask = "xxxxxxxx?xxxxxx?xxx?x";

	//this.Present = scanner.ScanModule("41 8B F0 8B FA 89 54 24 ?? 48 8B D9 48 89 4D ?? C6 44 24 ?? 00") - 0x37;
	SignatureScanner SigScanner2;
	char process2[128];
	strcpy_s(process2, "dxgi.dll");
	char process3[128];
	strcpy_s(process3, "ffxiv_dx11.exe");
	if (SigScanner2.GetProcess(process3)) {
		module mod = SigScanner2.GetModule(process2);
		dxgi_present_offset = SigScanner2.FindSignature(mod.dwBase, mod.dwSize, Present_Signature, Present_Mask);
		dxgi_present_offset = dxgi_present_offset - 0x37;
		Log() << "dxgi_present: " + int_to_hex(dxgi_present_offset) << std::endl;
	}
	SignatureScanner SigScanner;
	char process[128];
	strcpy_s(process, "ffxiv_dx11.exe");
	if (SigScanner.GetProcess(process)) {
		module mod = SigScanner.GetModule(process);
		SIZE_T PA1 = SigScanner.FindSignature(mod.dwBase, mod.dwSize, PA1_Signature, PA1_Mask) - mod.dwBase;
		SIZE_T PA2 = SigScanner.FindSignature(mod.dwBase, mod.dwSize, PA2_Signature, PA2_Mask) - mod.dwBase;
		SIZE_T PA3 = SigScanner.FindSignature(mod.dwBase, mod.dwSize, PA3_Signature, PA3_Mask) - mod.dwBase;
		SIZE_T BLEU = SigScanner.FindSignature(mod.dwBase, mod.dwSize, BLEU_Signature, BLEU_Mask) - mod.dwBase;
		SIZE_T OperateItem = SigScanner.FindSignature(mod.dwBase, mod.dwSize, OperateItem_Signature, OperateItem_Mask) - mod.dwBase - 0x5;
		SIZE_T Loadhouse = SigScanner.FindSignature(mod.dwBase, mod.dwSize, Loadhouse_Signature, Loadhouse_Mask) - mod.dwBase - 0x5;
		SIZE_T Viewmatrix = SigScanner.FindSignature(mod.dwBase, mod.dwSize, Viewmatrix_Signature, Viewmatrix_Mask) - mod.dwBase; // -0x5;
		SIZE_T Basehouse = SigScanner.FindSignature(mod.dwBase, mod.dwSize, Basehouse_Signature, Basehouse_Mask) - mod.dwBase;
		SIZE_T SavePreview = SigScanner.FindSignature(mod.dwBase, mod.dwSize, SavePreview_Signature, SavePreview_Mask) - mod.dwBase - 0x5;
		SIZE_T select = SigScanner.FindSignature(mod.dwBase, mod.dwSize, select_signature, select_mask) - mod.dwBase;
		SIZE_T layoutworld = SigScanner.FindSignature(mod.dwBase, mod.dwSize, layoutworld_signature, layoutworld_mask) - mod.dwBase;
		SIZE_T actortable = SigScanner.FindSignature(mod.dwBase, mod.dwSize, actortable_signature, actortable_mask) - mod.dwBase;
		SIZE_T housingmodule = SigScanner.FindSignature(mod.dwBase, mod.dwSize, housingmodule_signature, housingmodule_mask) - mod.dwBase;
		OffsetMgr::pa1Offset = PA1;
		OffsetMgr::pa2Offset = PA2;
		OffsetMgr::pa3Offset = PA3;
		OffsetMgr::blueOffset = BLEU;
		OffsetMgr::offset_OperateItem = OperateItem;
		OffsetMgr::offset_Loadhouse = Loadhouse;
		
		OffsetMgr::offset_SavePreview = SavePreview;
		OffsetMgr::offset_Select = GetOffsetFromOpCallOffset(select, hProcess, baseAdd);
		OffsetMgr::layoutWorld = GetOffsetFromOpMovRegOffset(layoutworld, hProcess, baseAdd);
		OffsetMgr::baseHouse = OffsetMgr::layoutWorld;
		OffsetMgr::offset_ViewMatrix = GetOffsetFromOpCallOffset(Viewmatrix, hProcess, baseAdd);
		OffsetMgr::ActortableBase = GetOffsetFromOpMovRegOffset(actortable, hProcess, baseAdd);
		OffsetMgr::housingmodule = GetOffsetFromOpMovRegOffset(housingmodule, hProcess, baseAdd);
		

		Log() << "<<<<<< offets:" << std::endl;
		Log() << int_to_hex(OffsetMgr::pa1Offset) << std::endl;
		Log() << int_to_hex(OffsetMgr::pa2Offset) << std::endl;
		Log() << int_to_hex(OffsetMgr::pa3Offset) << std::endl;
		Log() << int_to_hex(OffsetMgr::blueOffset) << std::endl;
		Log() << int_to_hex(OffsetMgr::offset_OperateItem) << std::endl;
		Log() << int_to_hex(OffsetMgr::offset_Loadhouse) << std::endl;
		Log() << int_to_hex(OffsetMgr::offset_ViewMatrix) << std::endl;
		Log() << int_to_hex(OffsetMgr::offset_SavePreview) << std::endl;
		Log() << int_to_hex(OffsetMgr::offset_Select) << std::endl;
		Log() << int_to_hex(OffsetMgr::layoutWorld) << std::endl;
		Log() << int_to_hex(OffsetMgr::baseHouse) << std::endl;
		Log() << int_to_hex(OffsetMgr::ActortableBase) << std::endl;
		Log() << int_to_hex(OffsetMgr::housingmodule) << std::endl;
		Log() << ">>>>>> offets" << std::endl;
		return 1;
	}
	else {
		return 0;
	}
}

int FFProcess::OpenFFXIVGetBaseAdd() {
	ProcessFind(_T("ffxiv_dx11.exe"));
	OpenProcessByPid();
	GetBaseAddressByPid();
	UpdateOffset();
	return 1;
}
SIZE_T FFProcess::GetBaseAdd() {
	return this->baseAdd;
}
#pragma endregion

#pragma region ReadWriteMemory

#pragma endregion


#pragma region Guizmo
bool FFProcess::IsHousing() {
	int state1 = ReadGameMemory<int>(this->hProcess, baseAdd, OffsetMgr::ActiveItemState1_hoverORmoverotate);
	Log() << "IsHousing:" << state1 << std::endl;
	return state1 != 0;
}
bool FFProcess::CanEdit() {
	// FIXME: test hook
	// return true;
	// ACTIVE NOTHING:0 HOVER:1 MOVE/ROTATE:3
	int state1 = ReadGameMemory<int>(this->hProcess, baseAdd, OffsetMgr::ActiveItemState1_hoverORmoverotate);
	// MOVE:1 or ROTATE:2
	int state2 = ReadGameMemory<int>(this->hProcess, baseAdd, OffsetMgr::ActiveItemState2_moveORrotate);
	if (state1 == 3 && state2 == 2) {
		return true;
	}
	else {
		return false;
	}
}

std::vector<float> FFProcess::GetSelectedItemPos() {
	if (CanEdit()) {
		POS pos = ReadGameMemory<POS>(this->hProcess, baseAdd,OffsetMgr::SJPosition);
		return std::vector<float>{pos.x, pos.y, pos.z};
	}
	else {
		return std::vector<float>{0.,0.,0.};
	}
}

std::vector<float> FFProcess::GetSelectedItemRotation() {
	if (CanEdit()) {
		QUA rotation = ReadGameMemory<QUA>(this->hProcess, baseAdd, OffsetMgr::SJRotation);
		return Qua2Deg(rotation);
	}
	else {
		return std::vector<float>{0.};
	}
}

void FFProcess::SetSelectedItemPos(std::vector<float> pos) {
	if (!CanEdit()) {
		return;
	}
	POS newpos;
	newpos.x = pos[0];
	newpos.y = pos[1];
	newpos.z = pos[2];
	WriteGameMemory<POS>(this->hProcess, baseAdd, newpos, OffsetMgr::SJPosition);
}

void FFProcess::SetSelectedItemRotation(std::vector<float> rotation) {
	if (!CanEdit()) {
		return;
	}
	WriteGameMemory<QUA>(this->hProcess, baseAdd, Euler2Qua(rotation), OffsetMgr::SJRotation);
}

bool FFProcess::isRotating() {
	//int state = ReadGameMemory<int>(baseAdd, OffsetMgr::IsRotating);
	int state = ReadGameMemory<int>(this->hProcess, baseAdd, OffsetMgr::IsRotating2);
	//return (state == 1);
	//cout << "state:" << state << endl;
	return !(state == 2 || state == 3 );
}
#pragma endregion

#pragma region Pos2
void FFProcess::SetPos2X(float x) {
	if (!CanEdit()) {
		return;
	}
	WriteGameMemory<float>(this->hProcess, baseAdd, x, OffsetMgr::SJPositionX);
}
void FFProcess::SetPos2Y(float y) {
	if (!CanEdit()) {
		return;
	}
	WriteGameMemory<float>(this->hProcess, baseAdd, y, OffsetMgr::SJPositionY);
}
void FFProcess::SetPos2Z(float z) {
	if (!CanEdit()) {
		return;
	}
	WriteGameMemory<float>(this->hProcess, baseAdd, z, OffsetMgr::SJPositionZ);
}
#pragma endregion


#pragma region MemoryPosRotation_1
std::vector<float> FFProcess::GetActiveItemPos() {
	std::vector<SIZE_T> offsets = OffsetMgr::ActiveItemPosition2;
	std::vector<SIZE_T> finalAddrOffsets{ 0x00,0x04,0x08 };
	SIZE_T addr1 = baseAdd;
	return ReadGameMemory<float>(this->hProcess, addr1, offsets, finalAddrOffsets);
}

int FFProcess::SetActiveItemX(float x) {
	std::vector<SIZE_T> offsets = OffsetMgr::ActiveItemPosition2;
	std::vector<SIZE_T> finalAddrOffsets{ 0x00,0x04,0x08 };
	SIZE_T addr1 = baseAdd;
	WriteGameMemory<float>(this->hProcess, addr1, x, offsets, finalAddrOffsets[0]);
	return 0;
}
int FFProcess::SetActiveItemY(float y) {
	std::vector<SIZE_T> offsets = OffsetMgr::ActiveItemPosition2;
	std::vector<SIZE_T> finalAddrOffsets{ 0x00,0x04,0x08 };
	SIZE_T addr1 = baseAdd;
	WriteGameMemory<float>(this->hProcess, addr1, y, offsets, finalAddrOffsets[1]);
	return 0;
}
int FFProcess::SetActiveItemZ(float z) {
	std::vector<SIZE_T> offsets = OffsetMgr::ActiveItemPosition2;
	std::vector<SIZE_T> finalAddrOffsets{ 0x00,0x04,0x08 };
	SIZE_T addr1 = baseAdd;
	WriteGameMemory<float>(this->hProcess, addr1, z, offsets, finalAddrOffsets[2]);
	return 0;
}
float FFProcess::GetActiveItemRotation() {
	std::vector<SIZE_T> offsets = OffsetMgr::ActiveItemRotation2;
	SIZE_T addr1 = baseAdd;
	return ReadGameMemory<float>(this->hProcess, addr1, offsets);
}
int FFProcess::SetActiveItemRotation(float r) {
	std::vector<SIZE_T> offsets = OffsetMgr::ActiveItemRotation2;
	SIZE_T addr1 = baseAdd;
	WriteGameMemory<float>(this->hProcess, addr1, r, offsets);
	return 0;
}

#pragma endregion

#pragma region PlaceAnywhere
int FFProcess::InjectPlaceAnywhere() {
	// TODO: unify memory operations
	SIZE_T _add1_value2 = 0x4c010000017387c6;
	SIZE_T _add2_value2 = 0x80010000017387c6;
	SIZE_T _add3_value2 = 0x48010000017387c6;
	SIZE_T _add1 = this->baseAdd + OffsetMgr::pa1Offset;
	SIZE_T _add2 = this->baseAdd + OffsetMgr::pa2Offset;
	SIZE_T _add3 = this->baseAdd + OffsetMgr::pa3Offset;
	SIZE_T dwSize;
	int wri_res1 = WriteProcessMemory(hProcess, (LPVOID)_add1, &_add1_value2, sizeof(SIZE_T), &dwSize);
	int wri_res2 = WriteProcessMemory(hProcess, (LPVOID)_add2, &_add2_value2, sizeof(SIZE_T), &dwSize);
	int wri_res3 = WriteProcessMemory(hProcess, (LPVOID)_add3, &_add3_value2, sizeof(SIZE_T), &dwSize);
	return wri_res1 && wri_res2;
}
int FFProcess::reInjectPlaceAnywhere() {
	SIZE_T _add1_value1 = 0x4c000000017387c6;
	SIZE_T _add2_value1 = 0x80000000017387c6;
	SIZE_T _add3_value1 = 0x48000000017387c6;
	SIZE_T _add1 = this->baseAdd + OffsetMgr::pa1Offset;
	SIZE_T _add2 = this->baseAdd + OffsetMgr::pa2Offset;
	SIZE_T _add3 = this->baseAdd + OffsetMgr::pa3Offset;
	SIZE_T dwSize;
	int wri_res1 = WriteProcessMemory(hProcess, (LPVOID)_add1, &_add1_value1, sizeof(SIZE_T), &dwSize);
	int wri_res2 = WriteProcessMemory(hProcess, (LPVOID)_add2, &_add2_value1, sizeof(SIZE_T), &dwSize);
	int wri_res3 = WriteProcessMemory(hProcess, (LPVOID)_add3, &_add3_value1, sizeof(SIZE_T), &dwSize);
	return wri_res1 && wri_res2;
}
#pragma endregion

#pragma region IMPORTEXPORT

int FFProcess::GetFurnitureListFromMemory() {
	vfmemory.clear();
	FFHook& ffhook = FFHook::get_instance();
	//std::cout << std::uppercase<<std::hex<<"ffhook.getLoadhouse_P1():" << ffhook.getLoadhouse_P1() << std::endl;
	__int64 StartAddr = ReadGameMemory<__int64>(this->hProcess, ffhook.getLoadhouse_P1()) + (__int64)0x8;
	//GJF
	//__int64 StartAddr = ReadGameMemory<__int64>((SIZE_T)0x0000022EC0364E30) + (__int64)0x8;
	//1A87C9889B0
	//__int64 StartAddr = 0x1A87C9889B0 + 0x8;
	SIZE_T _cc = 0;
	int _al = 0;
	while (_cc < 405) {
		__int64 start = StartAddr + _cc * (__int64)0x30;
		__int64 addr_x = start + 0x18;
		__int64 addr_y = addr_x + 0x4;
		__int64 addr_z = addr_x + 0x8;
		__int64 addr_r = addr_z + 0x8;
		__int64 addr_o = addr_r + 0x4;
		__int64 addr_color = addr_x - 0xc;
		__int64 addr_cate2 = addr_x - 0xf;
		__int64 addr_cate1 = addr_cate2 - 0x1;
		int cate = 0;
		unsigned char* pi = (unsigned char*)&cate;
		for (int j = 0; j < 4; j++) {
			if (j == 0)
				*(pi + j) = ReadGameMemory<char>(this->hProcess, addr_cate1);
			else if (j == 1)
				*(pi + j) = ReadGameMemory<char>(this->hProcess, addr_cate2);
		}
		int ord = ReadGameMemory<int>(this->hProcess, addr_o);

		int color = (int)ReadGameMemory<char>(this->hProcess, addr_color);
		float x = ReadGameMemory<float>(this->hProcess, addr_x);
		float y = ReadGameMemory<float>(this->hProcess, addr_y);
		float z = ReadGameMemory<float>(this->hProcess, addr_z);
		float r = ReadGameMemory<float>(this->hProcess, addr_r);
		if (ord == _al) {
			_al++;
			Log() << x << ", " << y << ", " << z << ", " << r << endl;
			vfmemory.push_back(FurnitureInstance(cate, x, y, z, r, color));
		}
		_cc++;
	}
	return 1;
}

int FFProcess::Boogiepop_GetFurnitureListFromMemory(std::vector<FurnitureInstance>& vf,__int64 loadhouseP1) {
	vf.clear();
	FFHook& ffhook = FFHook::get_instance();
	//__int64 StartAddr = ReadGameMemory<__int64>(ffhook.getLoadhouse_P1()) + (__int64)0x8;
	__int64 StartAddr = ReadGameMemory<__int64>(this->hProcess, loadhouseP1) + (__int64)0x8;
	/*
	std::vector<SIZE_T> offset_1st{ 0x01DD4CA0,0x10,0x8 };
	SIZE_T a1 = ReadGameMemory<SIZE_T>(GetBaseAdd() + offset_1st[0]);
	SIZE_T a2 = ReadGameMemory<SIZE_T>(a1 + offset_1st[1]);
	a2 = a2 + offset_1st[2];
	__int64 StartAddr = a2;
	*/
	SIZE_T _cc = 0;
	int _al = 0;
	while (_cc < 405) {
		__int64 start = StartAddr + _cc * (__int64)0x30;
		__int64 addr_x = start + 0x18;
		__int64 addr_y = addr_x + 0x4;
		__int64 addr_z = addr_x + 0x8;
		__int64 addr_r = addr_z + 0x8;
		__int64 addr_o = addr_r + 0x4;
		__int64 addr_color = addr_x - 0xc;
		__int64 addr_cate2 = addr_x - 0xf;
		__int64 addr_cate1 = addr_cate2 - 0x1;
		int cate = 0;
		unsigned char* pi = (unsigned char*)&cate;
		for (int j = 0; j < 4; j++) {
			if (j == 0)
				*(pi + j) = ReadGameMemory<char>(this->hProcess, addr_cate1);
			else if (j == 1)
				*(pi + j) = ReadGameMemory<char>(this->hProcess, addr_cate2);
		}
		int ord = ReadGameMemory<int>(this->hProcess, addr_o);

		int color = (int)ReadGameMemory<char>(this->hProcess, addr_color);
		float x = ReadGameMemory<float>(this->hProcess, addr_x);
		float y = ReadGameMemory<float>(this->hProcess, addr_y);
		float z = ReadGameMemory<float>(this->hProcess, addr_z);
		float r = ReadGameMemory<float>(this->hProcess, addr_r);
		if (ord == _al) {
			_al++;
			vf.push_back(FurnitureInstance(cate, x, y, z, r, color));
			Log() << "readmemory: cate = " << cate << " x,y,z,r = " << x << " " << y << " " << z << " " << r << std::endl;
		}
		_cc++;
	}
	return 1;
}

int FFProcess::FurList2CateList() {
	int _tm = -1;
	memset(catex, _tm, sizeof(catex));
	vcategory.clear();
	for (int i = 0; i < vfmemory.size(); i++) {
		if (catex[vfmemory[i].category] == -1) {
			CategoryInstance _tci(vfmemory[i].category);
			_tci.count = 1;
			_tci.posX.push_back(vfmemory[i].x);
			_tci.posY.push_back(vfmemory[i].y);
			_tci.posZ.push_back(vfmemory[i].z);
			_tci.r.push_back(vfmemory[i].r);
			_tci.color.push_back(vfmemory[i].color);
			vcategory.push_back(_tci);
			catex[vfmemory[i].category] = vcategory.size() - 1;
		}
		else {
			vcategory[catex[vfmemory[i].category]].count++;
			vcategory[catex[vfmemory[i].category]].posX.push_back(vfmemory[i].x);
			vcategory[catex[vfmemory[i].category]].posY.push_back(vfmemory[i].y);
			vcategory[catex[vfmemory[i].category]].posZ.push_back(vfmemory[i].z);
			vcategory[catex[vfmemory[i].category]].r.push_back(vfmemory[i].r);
			vcategory[catex[vfmemory[i].category]].color.push_back(vfmemory[i].color);
		}
	}

	return 1;
}

// TODO: refactor operations on json
int FFProcess::CateList2Json(const char* FilePath) {
	rapidjson::Document d;
	d.Parse("{}");
	d.AddMember("size", vcategory.size(), d.GetAllocator());
	rapidjson::Value d3(rapidjson::kArrayType);
	for (int j = 0; j < vcategory.size(); j++) {
		CategoryInstance temp = vcategory[j];
		char catId[4];
		_itoa_s(temp.categoryId, catId, 16);
		rapidjson::Value d2(rapidjson::kObjectType);
		d2.AddMember("categoryId", temp.categoryId, d.GetAllocator());
		d2.AddMember("count", temp.count, d.GetAllocator());
		//X
		rapidjson::Value arrX(rapidjson::kArrayType);
		for (int i = 0; i < temp.posX.size(); i++) {
			arrX.PushBack(temp.posX[i], d.GetAllocator());
		}
		d2.AddMember("posX", arrX, d.GetAllocator());
		//Y
		rapidjson::Value arrY(rapidjson::kArrayType);
		for (int i = 0; i < temp.posY.size(); i++) {
			arrY.PushBack(temp.posY[i], d.GetAllocator());
		}
		d2.AddMember("posY", arrY, d.GetAllocator());
		//Z
		rapidjson::Value arrZ(rapidjson::kArrayType);
		for (int i = 0; i < temp.posZ.size(); i++) {
			arrZ.PushBack(temp.posZ[i], d.GetAllocator());
		}
		d2.AddMember("posZ", arrZ, d.GetAllocator());
		//R
		rapidjson::Value arrR(rapidjson::kArrayType);
		for (int i = 0; i < temp.r.size(); i++) {
			arrR.PushBack(temp.r[i], d.GetAllocator());
		}
		d2.AddMember("Rotation", arrR, d.GetAllocator());
		//COLOR
		rapidjson::Value arrCOLOR(rapidjson::kArrayType);
		for (int i = 0; i < temp.color.size(); i++) {
			arrCOLOR.PushBack(temp.color[i], d.GetAllocator());
		}
		d2.AddMember("Color", arrCOLOR, d.GetAllocator());
		d3.PushBack(d2, d.GetAllocator());
	}
	d.AddMember("list", d3, d.GetAllocator());
	//JSON d
	FILE* fp_w;
	fopen_s(&fp_w, FilePath, "w+");
	char writeBuffer[512];
	rapidjson::FileWriteStream os(fp_w, writeBuffer, sizeof(writeBuffer));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	d.Accept(writer);
	fclose(fp_w);
	return 1;
}
#pragma endregion

#pragma region IO
std::string FFProcess::GetLocalAppDataPath() {
	WCHAR path[MAX_PATH];
	SHGetSpecialFolderPath(NULL, path, CSIDL_APPDATA, FALSE);
	std::string appdatapath;
	Wchar_tToString(appdatapath, path);
	return appdatapath;
}
int FFProcess::LocalAppDataExist() {
	std::string strpath = GetLocalAppDataPath() + "\\HaiBinQiangJia";
	if (0 != _access(strpath.c_str(), 0))
	{
		_mkdir(strpath.c_str());
		return 1;
	}
	return 0;
}
int FFProcess::CateList2JsonFileTemp() {
	write_catelist = 0;
	LocalAppDataExist();
	std::string strpath = GetLocalAppDataPath() + "\\HaiBinQiangJia\\__tempcate.hbqj";
	CateList2Json(strpath.c_str());
	write_catelist = 1;
	return 1;
}
//GJF
int FFProcess::ExportCateJsonTempFile() {
	bool bbb = IsHousing();
	//bool bbb = true;
	bbb = true;
	int state1 = ReadGameMemory<int>(this->hProcess, baseAdd, OffsetMgr::ActiveItemState1_hoverORmoverotate);
	Log() << "before" << std::endl;
	if (bbb) {
		
		GetFurnitureListFromMemory();

		FurList2CateList();

		CateList2JsonFileTemp();

	}
	return 1;
}

std::vector<CategoryInstance> FFProcess::ReadCateListFromJson(const char* FilePath) {
	std::vector<CategoryInstance> res;
	if ((_access(FilePath, 0)) == -1) {
		return res;
	}
	FILE* fp_r;
	fopen_s(&fp_r, FilePath, "r+");
	char readBuffer[512];
	rapidjson::FileReadStream is(fp_r, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(is);
	fclose(fp_r);
	remove(FilePath);
	int listSize = d["size"].GetInt();
	for (int i = 0; i < listSize; i++) {
		CategoryInstance temp;
		temp.categoryId = d["list"][i]["categoryId"].GetInt();
		temp.count = d["list"][i]["count"].GetInt();
		for (int j = 0; j < temp.count; j++) {
			temp.posX.push_back(d["list"][i]["posX"][j].GetDouble());
			temp.posY.push_back(d["list"][i]["posY"][j].GetDouble());
			temp.posZ.push_back(d["list"][i]["posZ"][j].GetDouble());
			temp.r.push_back(d["list"][i]["Rotation"][j].GetDouble());
			temp.color.push_back(d["list"][i]["Color"][j].GetInt());
		}
		res.push_back(temp);
	}
	return res;
}

std::vector<CategoryInstance> FFProcess::ReadTempCateList() { //no ref
	std::string strpath = GetLocalAppDataPath() + "\\HaiBinQiangJia\\__tempcate.hbqj";
	return ReadCateListFromJson(strpath.c_str());
}

std::vector<CategoryInstance> FFProcess::Boogiepop_Import_ReadCateList() {
	std::string strpath = GetLocalAppDataPath() + "\\HaiBinQiangJia\\__boogiepop_import.hbqj";
	return ReadCateListFromJson(strpath.c_str());
}

std::vector<CategoryInstance> FFProcess::Boogiepop_Preview_ReadCateList() {
	std::string strpath = GetLocalAppDataPath() + "\\HaiBinQiangJia\\__boogiepop_preview.hbqj";
	return ReadCateListFromJson(strpath.c_str());
}
#pragma endregion


#pragma region Boogiepop
int FFProcess::Load_Inject(const wchar_t* dllpath) {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.LoadDLLGetFunc(dllpath);
	Log() << "before InjectDLL,pid=" + std::to_string(pid) << endl;
	ffhook.InjectDLL(dllpath, pid);
	return pid;
}
int FFProcess::FP_Boogiepop_Preview() {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.PreviewOnce();
	return 1;
}
//Guizmo
int FFProcess::FP_Boogiepop_OpenGuizmo() {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.OpenGuizmo();
	return 1;
}
int FFProcess::FP_Boogiepop_CloseGuizmo() {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.CloseGuizmo();
	return 1;
}
int FFProcess::FP_Boogiepop_OpenGuizmoSnap() {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.OnGuizmoSnap();
	return 1;
}
int FFProcess::FP_Boogiepop_CloseGuizmoSnap() {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.OffGuizmoSnap();
	return 1;
}
int FFProcess::FP_Boogiepop_SetGuizmoSnap(float s) {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.SetGuizmoSnap(s);
	return 1;
}
int FFProcess::FP_Boogiepop_BeginArranging() {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.BeginArrange();
	return 1;
}
int FFProcess::FP_Boogiepop_GetImportProgress() {
	FFHook& ffhook = FFHook::get_instance();
	int _c = ffhook.GetImportProgress();
	return _c;
}
#pragma endregion

#endif

void FFProcess::LoadInjectBoogiepop(){
	//FFHook& ffhook = FFHook::get_instance();
	std::string strpath = GetLocalAppDataPath() + "\\HaiBinQiangJia\\boogiepop.dll";
	std::wstring ws = stringToWstring(strpath);
	Log() << "DLL:" + strpath << endl;
	if (Load_Inject(ws.c_str()) != 0) {
		Log() << "InejctSuc" << endl;
	}
}

int FFProcess::FP_Boogiepop_OnSavePreview() {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.SetSavePreview(1);
	return 1;
}
int FFProcess::FP_Boogiepop_OffSavePreview() {
	FFHook& ffhook = FFHook::get_instance();
	ffhook.SetSavePreview(0);
	return 1;
}


//ActorTable
std::vector<char> FFProcess::GetActorName() {
	//OffsetMgr::ActorName
	SIZE_T addr1 = ReadGameMemory<SIZE_T>(this->hProcess, GetBaseAdd() + OffsetMgr::ActorName[0]);
	SIZE_T addr2 = addr1 + OffsetMgr::ActorName[1];
	//addr2 24byte
	char buf[24];
	SIZE_T readSz;
	ReadProcessMemory(this->hProcess, (LPCVOID)addr2, &buf, sizeof(char)*24, &readSz);
	std::vector<char> res;
	res.insert(res.end(), buf, buf + 24);
	return res;
}
int FFProcess::GetActorHomeworld() {
	SIZE_T addr1 = ReadGameMemory<SIZE_T>(this->hProcess, GetBaseAdd() + OffsetMgr::ActorHomeworld[0]);
	int res = (short)ReadGameMemory<short>(this->hProcess, addr1 + OffsetMgr::ActorHomeworld[1]);
	return res;
}
int FFProcess::GetActorId() {
	SIZE_T addr1 = ReadGameMemory<SIZE_T>(this->hProcess, GetBaseAdd() + OffsetMgr::ActorId[0]);
	int res = ReadGameMemory<int>(this->hProcess, addr1 + OffsetMgr::ActorId[1]);
	return res;
}

void FFProcess::SetTime(unsigned int time) {
	//const unsigned int UMAX = 4294967295;
	SIZE_T addr = GetBaseAdd() + 0xC2C290 + 0x19 + 0x3;
	SIZE_T dwSize;

	char newTime[] = { 0x49, 0xC7, 0xC1, 0x00, 0x00, 0x00, 0x00 };
	WriteProcessMemory(this->hProcess, (LPVOID)(GetBaseAdd() + 0xC2C290 + 0x19), &newTime, sizeof(char) * 7, &dwSize);
	
	WriteProcessMemory(this->hProcess, (LPVOID)(addr), &time, sizeof(time), &dwSize);
	//Log() << "SetTime to" + std::to_string(time) + " ,dwSize = " + std::to_string(dwSize) << std::endl;
}

void FFProcess::DisableSetTime() {
	SIZE_T dwSize;
	char newTime[] = { 0x4D, 0x8B, 0x8A, 0x70, 0x17, 0x00, 0x00 };
	WriteProcessMemory(this->hProcess, (LPVOID)(GetBaseAdd() + 0xC2C290 + 0x19), &newTime, sizeof(char) * 7, &dwSize);
}




void FFProcess::import() {
	LoadInjectBoogiepop();
	Sleep(2000);
	Log() << "begin importing" << std::endl;
	auto list = get_furniture_addr_list();
	init_my_select();
	total_count = list.size();
	InjectPlaceAnywhere();
	LayoutWorld* layoutWorld = (LayoutWorld*)*(SIZE_T*)(baseAdd + OffsetMgr::layoutWorld);
	std::vector<CategoryInstance> import_file_furniture_list = Boogiepop_Import_ReadCateList();
	Log() << "import_file_furniture_list:" << import_file_furniture_list.size() << endl;
	FFHook& hook = FFHook::get_instance();
	hook.set_Total_count(total_count);
	hook.set_Import_c(0);
	for (auto _addr : list) {
		HousingGameObject* obj = (HousingGameObject*)_addr;
		int target_cate = obj->category;
		int target_color = obj->color;
		float px = 0, py = 0, pz = 0, pr = 0;
		for (auto& ci : import_file_furniture_list) {
			if (ci.categoryId == target_cate && ci.color.size() > 0) {
				int i = 0;
				for (; i < ci.color.size(); i++) {
					if (ci.color[i] == target_color) {
						break;
					}
				}
				if (i == ci.color.size()) {
					i--;
				}
				if (i >= 0) {
					px = ci.posX[i]; ci.posX.erase(ci.posX.begin() + i);
					py = ci.posY[i]; ci.posY.erase(ci.posY.begin() + i);
					pz = ci.posZ[i]; ci.posZ.erase(ci.posZ.begin() + i);
					pr = ci.r[i];	 ci.r.erase(ci.r.begin() + i);
					ci.color.erase(ci.color.begin() + i);
					ci.count--;
				}
				break;
			}
		}

		auto qua = Euler2Qua({ 0, pr * Rad2Deg,0 });
		set_rotate_mode();
		my_select(housing, obj->addr);
		Log() << "my_select()" << endl;
		layoutWorld->HousingStruct->ActiveItem->x = px;
		layoutWorld->HousingStruct->ActiveItem->y = py;
		layoutWorld->HousingStruct->ActiveItem->z = pz;
		layoutWorld->HousingStruct->ActiveItem->qw = qua.w;
		layoutWorld->HousingStruct->ActiveItem->qy = qua.y;
		layoutWorld->HousingStruct->ActiveItem->qx = 0;
		layoutWorld->HousingStruct->ActiveItem->qz = 0;

		send_click(window);
		import_count++;
		hook.set_Import_c(import_count);
		Sleep(1500);
	}
}

