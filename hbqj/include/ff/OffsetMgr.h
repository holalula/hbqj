#ifndef __OFFSETMGR_H
#define __OFFSETMGR_H
#pragma once
#include "../../pch.h"
#include <vector>

static class DLLAPI OffsetMgr {
public:
	static std::vector<SIZE_T> ActiveItemPosition2;
	static std::vector<SIZE_T> ActiveItemRotation2;
	static SIZE_T IsWarehouseOffset;
	//Boogiepop
	static __int64 offset_OperateItem;
	static __int64 offset_Loadhouse;
	static __int64 offset_ViewMatrix;
	static __int64 offset_Select;
	
	static SIZE_T pa1Offset;
	static SIZE_T pa2Offset;
	static SIZE_T pa3Offset;
	static SIZE_T blueOffset;
	static std::vector<SIZE_T> ActiveItemState1_hoverORmoverotate; //ACTIVE NOTHING:0 HOVER:1 MOVE/ROTATE:3
	static std::vector<SIZE_T> ActiveItemState2_moveORrotate; //MOVE:1 or ROTATE:2

	static std::vector<SIZE_T> SJPosition;
	static std::vector<SIZE_T> SJRotation;
	static std::vector<SIZE_T> SJPositionX;
	static std::vector<SIZE_T> SJPositionY;
	static std::vector<SIZE_T> SJPositionZ;

	static std::vector<SIZE_T> IsRotating;
	static std::vector<SIZE_T> IsRotating2;

	static SIZE_T baseHouse;

	static __int64 offset_SavePreview;

	static SIZE_T ActortableBase;
	static std::vector<SIZE_T> ActorName;
	static std::vector<SIZE_T> ActorHomeworld;
	static std::vector<SIZE_T> ActorId;

	static std::vector<SIZE_T> Furniture_List;

	static SIZE_T layoutWorld;
	static SIZE_T housingmodule;
};
// TODO: implement memory operations by offets or by models?
SIZE_T OffsetMgr::IsWarehouseOffset = 0x08 - 0x10;
SIZE_T OffsetMgr::pa1Offset = 0;
SIZE_T OffsetMgr::pa2Offset = 0;
SIZE_T OffsetMgr::pa3Offset = 0;
SIZE_T OffsetMgr::blueOffset = 0;
__int64 OffsetMgr::offset_OperateItem = 0;
__int64 OffsetMgr::offset_Loadhouse = 0;
__int64 OffsetMgr::offset_ViewMatrix = 0;
__int64 OffsetMgr::offset_SavePreview = 0;
__int64 OffsetMgr::offset_Select = 0;
SIZE_T OffsetMgr::layoutWorld = 0;
/*SavePreview  0x572b60
* Address of signature = ffxiv_dx11.exe + 0x00572B65
"\x48\x89\x00\x00\x00\x48\x89\x00\x00\x00\x55\x48\x8D\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x00\x00\x48\x33\x00\x48\x89\x00\x00\x0F\x10", "xx???xx???xxx???xxx????xx?????xx?xx??xx"
"48 89 ? ? ? 48 89 ? ? ? 55 48 8D ? ? ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 0F 10"
*/
SIZE_T OffsetMgr::baseHouse = 0;
std::vector<SIZE_T> OffsetMgr::ActiveItemPosition2{ OffsetMgr::baseHouse,0x40,0x80,0x110 };  
std::vector<SIZE_T> OffsetMgr::ActiveItemRotation2{ OffsetMgr::baseHouse,0x40,0x80,0x124 }; 
std::vector<SIZE_T> OffsetMgr::ActiveItemState1_hoverORmoverotate{ OffsetMgr::baseHouse, 0x40, 0x8 }; //ACTIVE NOTHING:0 HOVER:1 MOVE/ROTATE:3
std::vector<SIZE_T> OffsetMgr::ActiveItemState2_moveORrotate{ OffsetMgr::baseHouse, 0x40, 0x0 }; //MOVE:1 or ROTATE:2
std::vector<SIZE_T> OffsetMgr::SJPosition{ OffsetMgr::baseHouse, 0x40, 0x18, 0x50 };
std::vector<SIZE_T> OffsetMgr::SJRotation{ OffsetMgr::baseHouse, 0x40, 0x18, 0x60 };
std::vector<SIZE_T> OffsetMgr::IsRotating{ OffsetMgr::baseHouse, 0x40, 0xb8 };
std::vector<SIZE_T> OffsetMgr::IsRotating2{ OffsetMgr::baseHouse, 0x40, 0x90 };

std::vector<SIZE_T> OffsetMgr::SJPositionX{ OffsetMgr::baseHouse, 0x40, 0x18, 0x50 };
std::vector<SIZE_T> OffsetMgr::SJPositionY{ OffsetMgr::baseHouse, 0x40, 0x18, 0x50 +0x4};
std::vector<SIZE_T> OffsetMgr::SJPositionZ{ OffsetMgr::baseHouse, 0x40, 0x18, 0x50 +0x8};

SIZE_T OffsetMgr::housingmodule = 0;
std::vector<SIZE_T> OffsetMgr::Furniture_List{ OffsetMgr::housingmodule , 0x10, 0x8980 };

SIZE_T OffsetMgr::ActortableBase = 0;
std::vector<SIZE_T> OffsetMgr::ActorName{ OffsetMgr::ActortableBase,0x30 };
std::vector<SIZE_T> OffsetMgr::ActorHomeworld{ OffsetMgr::ActortableBase,0x195E };
std::vector<SIZE_T> OffsetMgr::ActorId{ OffsetMgr::ActortableBase,0x30 };
#endif