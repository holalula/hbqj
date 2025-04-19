#pragma once
#include "include/ff/FFProcess.h"
#include "include/utils/memory.h"
#define DLLEXPORT extern "C" __declspec(dllexport)

DLLEXPORT void OpenFFXIVProcess() {
	FFProcess& fp = FFProcess::get_instance();
	fp.OpenFFXIVGetBaseAdd();
}

#pragma region ActorTable
DLLEXPORT int GameOn() {
	FFProcess& fp = FFProcess::get_instance();
	if (fp.GetBaseAdd() == 0) {
		return 0;
	}
	__int64 actor = Memory::ReadGameMemory<__int64>(fp.hProcess, fp.GetBaseAdd() + OffsetMgr::ActortableBase);
	if (actor == 0) {
		return 0;
	}
	else {
		return 1;
	}
}
DLLEXPORT char* GetActorName(char* name) {
	FFProcess& fp = FFProcess::get_instance();
	std::vector<char> res = fp.GetActorName();
	for (int i = 0; i < res.size(); i++) {
		name[i] = res[i];
	}
	return name;
}
DLLEXPORT int GetActorHomeworld() {
	FFProcess& fp = FFProcess::get_instance();
	return fp.GetActorHomeworld();
}
DLLEXPORT int GetActorId() {
	FFProcess& fp = FFProcess::get_instance();
	return fp.GetActorId();
}
#pragma endregion



#pragma region PlaceAnywhere
DLLEXPORT void OnPlaceAnywhere() {
	FFProcess& fp = FFProcess::get_instance();
	fp.InjectPlaceAnywhere();
}
DLLEXPORT void OffPlaceAnywhere() {
	FFProcess& fp = FFProcess::get_instance();
	fp.reInjectPlaceAnywhere();
}
#pragma endregion

#pragma region t1Pos
DLLEXPORT float* GetPos1(float* pos) {
	FFProcess& fp = FFProcess::get_instance();
	std::vector<float> p = fp.GetActiveItemPos();
	for (int i = 0; i < 3; i++) {
		pos[i] = p[i];
	}
	return pos;
}
DLLEXPORT void SetPosX1(float x) {
	FFProcess& fp = FFProcess::get_instance();
	fp.SetActiveItemX(x);
}
DLLEXPORT void SetPosY1(float y) {
	FFProcess& fp = FFProcess::get_instance();
	fp.SetActiveItemY(y);
}
DLLEXPORT void SetPosZ1(float z) {
	FFProcess& fp = FFProcess::get_instance();
	fp.SetActiveItemZ(z);
}
#pragma endregion

#pragma region t2Pos
DLLEXPORT void SetPosX2(float x) {
	FFProcess& fp = FFProcess::get_instance();
	fp.SetPos2X(x);
}
DLLEXPORT void SetPosY2(float y) {
	FFProcess& fp = FFProcess::get_instance();
	fp.SetPos2Y(y);
}
DLLEXPORT void SetPosZ2(float z) {
	FFProcess& fp = FFProcess::get_instance();
	fp.SetPos2Z(z);
}
DLLEXPORT float* GetPos2(float* pos) {
	FFProcess& fp = FFProcess::get_instance();
	std::vector<float> p = fp.GetSelectedItemPos();
	for (int i = 0; i < 3; i++) {
		pos[i] = p[i];
	}
	return pos;
}
DLLEXPORT int CanEdit() {
	FFProcess& fp = FFProcess::get_instance();
	return fp.CanEdit();
}
#pragma endregion

#pragma region Guizmo
DLLEXPORT void OnGuizmo() {
	FFProcess& fp = FFProcess::get_instance();
	fp.FP_Boogiepop_OpenGuizmo();
}
DLLEXPORT void OffGuizmo() {
	FFProcess& fp = FFProcess::get_instance();
	fp.FP_Boogiepop_CloseGuizmo();
}
DLLEXPORT void OnGuizmoSnap() {
	FFProcess& fp = FFProcess::get_instance();
	fp.FP_Boogiepop_OpenGuizmoSnap();
}
DLLEXPORT void OffGuizmoSnap() {
	FFProcess& fp = FFProcess::get_instance();
	fp.FP_Boogiepop_CloseGuizmoSnap();
}
DLLEXPORT void SetGuizmoSnap(float s) {
	FFProcess& fp = FFProcess::get_instance();
	fp.FP_Boogiepop_SetGuizmoSnap(s);
}
#pragma endregion

#pragma region ImportExport
DLLEXPORT int IsHousingOn() {
	FFProcess& fp = FFProcess::get_instance();
	return fp.IsHousing();
}
DLLEXPORT int ExportHousing() {
	FFProcess& fp = FFProcess::get_instance();
	fp.ExportCateJsonTempFile();
	return 1;
}
DLLEXPORT int PreviewHousing() {
	FFProcess& fp = FFProcess::get_instance();
	fp.FP_Boogiepop_Preview();
	return 1;
}
DLLEXPORT int BeginArranging() {
	FFProcess& fp = FFProcess::get_instance();
	fp.FP_Boogiepop_BeginArranging();
	return 1;
}
DLLEXPORT int GetImportProgress() {
	FFProcess& fp = FFProcess::get_instance();
	return fp.FP_Boogiepop_GetImportProgress();
}
#pragma endregion


DLLEXPORT void LoadInjectBoogiepop() {
	FFProcess& fp = FFProcess::get_instance();
	fp.LoadInjectBoogiepop();
}


DLLEXPORT void OnSavePreview() {
	FFProcess& fp = FFProcess::get_instance();
	fp.FP_Boogiepop_OnSavePreview();
}
DLLEXPORT void OffSavePreview() {
	FFProcess& fp = FFProcess::get_instance();
	fp.FP_Boogiepop_OffSavePreview();
}

DLLEXPORT void DisableSetTime() {
	FFProcess& fp = FFProcess::get_instance();
	fp.DisableSetTime();
}

DLLEXPORT void SetTime(int time) {
	FFProcess& fp = FFProcess::get_instance();
	fp.SetTime(time);
}