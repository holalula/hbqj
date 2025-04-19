#pragma once
#include <vector>
struct FurnitureInstance {
	int category;
	int color;
	float x, y, z, r;
	FurnitureInstance(int c, float x, float y, float z, float r, int color) :category(c), x(x), y(y), z(z), r(r), color(color) {}
};
struct CategoryInstance {
	CategoryInstance(int categoryId) { this->categoryId = categoryId; this->count = 0; this->_p = 0; };
	CategoryInstance() {};
	int categoryId;
	int count;
	std::vector<float> posX;
	std::vector<float> posY;
	std::vector<float> posZ;
	std::vector<float> r;
	std::vector<int> color;
	int _p = 0;
};

//HOOKING
struct HousingItem {
	char _f1[0x50];
	float x;
	float y;
	float z;
	float _f2;
	float qx;
	float qy;
	float qz;
	float qw; //total size: 0x70
};
struct HousingStructure {
	char _f1[0x18];
	HousingItem* ActiveItem;
};
struct LayoutWorld {
	char _f1[0x40];
	HousingStructure* HousingStruct;
};

/*
* 0x30
* 0x40 (byte Name[64], para):0x30
* 0x10
* 0x4 (uint HousingRowId):0x80
* 0x1C
* 0x4(float X):0xA0
* 0x4(float Y):0xA4
* 0x4(float Z):0xA8
* 0x4C
* (int64_t pointer):0xF8
*/
struct HousingGameObject {
	char f1[0x30];
	char p1[0x40];
	char f2[0x10];
	short category;
	short _fill;
	//unsigned int category;
	char f3[0x1C];
	float x;
	float y;
	float z;
	char f4[0x4C];
	unsigned long long addr;
	char f5[0xb0];
	char color;
};

struct FurnitureList {
	unsigned long long objs[400];
};