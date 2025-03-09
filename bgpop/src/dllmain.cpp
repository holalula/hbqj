// TODO: refactor the hook
#include<WinSock2.h>

#include "include/mhook/mhook-lib/mhook.h"
#include "utils/memory.h"
#include "ff/FFProcess.h"
#include "ff/FFLog.h"

#pragma data_seg("DLLSharedSection") 
__int64 OPITEM_P1 = 0;
__int64 OPITEM_P2 = 0;
__int64 LOADHOUSE_P1 = 0;
__int64 LOADHOUSE_P2 = 0;
//IMPORT
int gInit = 0;
int gMode = 0;
int import_c = 0;
int total_count = 0;
//PREVIEW
int start_load = 0;
//Guizmo
int guizmoFlag = 0;
int guizmoSnapFlag = 0;
float guizmoSnap = 0.f;
//ViewMatrix
__int64 MatrixAddr = 0;
//FurPreview
int savepreview = 0;
#pragma data_seg()
#pragma comment(linker, "/Section:DLLSharedSection,rws") 

#define DLLEXPORT extern "C" _declspec(dllexport)
#pragma region DLLGETSETMETHODS
//new
DLLEXPORT void set_import_c(int _t) {
	import_c = _t;
}
DLLEXPORT void set_total_count(int _t) {
	total_count = _t;
}

DLLEXPORT void setInit(int _t) {
	gInit = _t;
}
DLLEXPORT int getInit() {
	return gInit;
}
DLLEXPORT int get_import_c() {
	return import_c;
}
DLLEXPORT int get_total_count() {
	return  total_count;
}
DLLEXPORT void setMode(int _t) {
	gMode = _t;
}
DLLEXPORT int getMode() {
	return gMode;
}
//PREVIEW
DLLEXPORT int getStartLoad() {
	return start_load;
}
DLLEXPORT void setStartLoad(int _t) {
	start_load = _t;
}
//OPERATE
DLLEXPORT __int64 getOPITEM_P1() {
	return OPITEM_P1;
}
DLLEXPORT __int64 getLOADHOUSE_P1() {
	return LOADHOUSE_P1;
}
//Guizmo
DLLEXPORT void setguizmoFlag(int _t) {
	guizmoFlag = _t;
}
DLLEXPORT int getguizmoFlag() {
	return guizmoFlag;
}
DLLEXPORT void setguizmoSnapFlag(int _t) {
	guizmoSnapFlag = _t;
}
DLLEXPORT int getguizmoSnapFlag() {
	return guizmoSnapFlag;
}
DLLEXPORT void setguizmoSnap(float _s) {
	guizmoSnap = _s;
}
//FurPreview

DLLEXPORT void setSavePreview(int _t) {
	savepreview = _t;
}
DLLEXPORT int getSavePreview() {
	return savepreview;
}
#pragma endregion

TCHAR pCallFileName[MAX_PATH];
const char* callName_ff = "ffxiv_dx11.exe";

#pragma region SavePreview
typedef void(__fastcall* _savepreview)(__int64* a, __int64* b);
_savepreview g_trueSavepreview;
int preview_once = 2;
static void __fastcall hook_savepreview(__int64* a, __int64* b) {
	if (savepreview ==1) {
		g_trueSavepreview(a, b);
		//preview_once--;
	}
	return g_trueSavepreview(a, b);
}
#pragma endregion


#pragma region ViewMatrix
typedef __int64 (WINAPI* _matrix)();
_matrix g_trueMatrix;
static __int64 WINAPI hook_matrix() {
	__int64 r = (__int64)g_trueMatrix();
	MatrixAddr = r + (__int64)0x1b4;
	return r;
}
#pragma endregion

#pragma region NetworkDef
typedef void (WINAPI* mhook_func)(char* buf, int len);
typedef int (WINAPI* _send)(SOCKET s, const char* buf, int len, int flags);
typedef int (WINAPI* _recv)(SOCKET s, char* buf, int len, int flags);
_send g_trueSend = (_send)GetProcAddress(GetModuleHandleA("WS2_32"), "send");
_recv g_trueRecv = (_recv)GetProcAddress(GetModuleHandleA("WS2_32"), "recv");
mhook_func _msend = NULL;
mhook_func _mrecv = NULL;

//WSA
typedef int (WINAPI* t_WSARecv)(SOCKET, LPWSABUF, DWORD, LPDWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
typedef int (WINAPI* t_WSASend)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);

t_WSASend o_WSASend = (t_WSASend)GetProcAddress(GetModuleHandleA("WS2_32"), "WSASend");
t_WSARecv o_WSARecv = (t_WSARecv)GetProcAddress(GetModuleHandleA("WS2_32"), "WSARecv");
int WINAPI hook_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	MessageBox(HWND_DESKTOP, L"Blabla", L"Titel", MB_OK);
	return o_WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
}

int WINAPI hook_WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	MessageBox(HWND_DESKTOP, L"Blabla", L"Titel", MB_OK);
	return o_WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}
#pragma endregion

#pragma region OperateLoadhouseDef
typedef void (WINAPI* _house)(void* housing, void* item);
_house g_trueOperate;
_house g_trueLoadhouse;
//preview housing
int read_json_once = 1;
char inputhousing[9600];
int load_num = 0;
int total_load_num = 0;
#pragma endregion

#pragma region ImportDef
std::vector<FurnitureInstance> vfurins;
int _once = 0;
#pragma endregion

#pragma region BDTH
typedef void (__fastcall*_select)(unsigned int* housing, __int64 item);
_select g_true_select;
static void __fastcall hook_select(unsigned int* housing, __int64 item) {
	Log() << "select:" << housing << ", " << item << std::endl;
	//item is HousingGameObject.addr
	//FFProcess& fp = FFProcess::get_instance();
	//std:vector<unsigned long long> furniture_addr_list = fp.get_furniture_addr_list();
	//HousingGameObject *obj = (HousingGameObject*)furniture_addr_list[0];
	//std::cout << "obj->addr:" << obj->addr << std::endl;
	//fp.SetActiveItemX(0); fp.SetActiveItemY(2); fp.SetActiveItemZ(0); fp.SetActiveItemRotation(1.5);
	//fp.SetPos2X(0); fp.SetPos2Y(2); fp.SetPos2Z(0);
	return g_true_select(housing, item);
}



#pragma endregion

#pragma region NetworkImport
void InitImport() {
	//Get order in mem
	//Set pos from __boogiepop
	FFProcess& fp = FFProcess::get_instance();
	vfurins.clear();
	std::vector<CategoryInstance> cateList = fp.Boogiepop_Import_ReadCateList();
	fp.Boogiepop_GetFurnitureListFromMemory(vfurins,LOADHOUSE_P1);
	for (int i = 0; i < vfurins.size(); i++) {
		int _cate = vfurins[i].category;
		for (int j = 0; j < cateList.size(); j++) {
			if (cateList[j].categoryId == _cate) {
				vfurins[i].x = cateList[j].posX[cateList[j]._p];
				vfurins[i].y = cateList[j].posY[cateList[j]._p];
				vfurins[i].z = cateList[j].posZ[cateList[j]._p];
				vfurins[i].r = cateList[j].r[cateList[j]._p];
				vfurins[i].color = cateList[j].color[cateList[j]._p];
				cateList[j]._p++;
			}
		}
	}
	total_count = vfurins.size();
	import_c = 0; _once = 0;
	MessageBox(NULL, TEXT("InitImport Completed"), TEXT("InitImport"), MB_ICONINFORMATION);
}
static int WINAPI hook_send(SOCKET s, const char* buf, int len, int flags) {
	return g_trueSend(s, buf, len, flags);
}

// Deprecated
//static int WINAPI hook_send(SOCKET s, const char* buf, int len, int flags)
//{
//	if (gInit == 1) {
//		InitImport();
//		gMode = 3;
//		gInit = 0;
//	}
//	unsigned char* pp = (unsigned char*)buf;
//	/*
//	if (len == 68 && (*(pp + 58) == 0x34 && *(pp + 59) == 0x01)) {
//		for (int i = 0; i < 68; i++) {
//			std::cout << std::hex << (int)*(pp + i) << " ";
//			if (i % 10 == 0) std::cout << std::endl;
//			
//		}
//	}
//	*/
////	if (len > 59 && (*(pp + 58) == 0x34 && *(pp + 59) == 0x01)) {	
//	if (len == 0x68 && (*(pp + 58) == 0x34 && *(pp + 59) == 0x01)) {
//		/*
//		for (int i = 0; i < 104; i++) {
//			std::cout << std::hex << (int)*(pp + i) << " ";
//			if ((i + 1) % 10 == 0) std::cout << std::endl;
//
//		}std::cout << std::endl;
//		std::cout << "(fur)len = " << len << " (len(buf) = "<< strlen(buf) << std::endl;
//		*/
//		//MessageBox(NULL, TEXT("HIT"), TEXT("hit here!"), MB_ICONINFORMATION);
//		if (gMode == 3) {
//			if (import_c <= total_count) {
//				if (import_c <= 255) {
//					char _n = 0x00 + (import_c) * 0x1;
//					*(pp + 80) = _n;
//				}
//				else {
//					//_c > 256
//					*(pp + 81) = 0x1;
//					unsigned char cc = 0x00 + (import_c - 256) * 0x1;
//					*(pp + 80) = cc;
//				}
//				/*
//				unsigned char* _px = (unsigned char*)&vfurins[import_c].x;
//				unsigned char* _py = (unsigned char*)&vfurins[import_c].y;
//				unsigned char* _pz = (unsigned char*)&vfurins[import_c].z;
//				unsigned char* _r = (unsigned char*)&vfurins[import_c].r;
//				*/
//				//std::cout << "Write Rotation: r=" << vfurins[import_c].r << std::endl;
//				/*
//				for (int _b = 0; _b < 4; _b++) {
//					*(pp + 84 + _b) = *(_px + _b);
//					*(pp + 88 + _b) = *(_py + _b);
//					*(pp + 92 + _b) = *(_pz + _b);
//					*(pp + 96 + _b) = *(_r + _b);
//					std::cout << "Write Rotation: rb=" << hex << (int)*(_r + _b) << " ";
//				}std::cout<<std::endl;
//				*/
//				memcpy(pp + 84, (unsigned char*)&vfurins[import_c].x, 4);
//				memcpy(pp + 88, (unsigned char*)&vfurins[import_c].y, 4);
//				memcpy(pp + 92, (unsigned char*)&vfurins[import_c].z, 4);
//				memcpy(pp + 96, (unsigned char*)&vfurins[import_c].r, 4);
//				import_c++;
//				if (import_c == total_count) {
//					if (_once == 0) {
//						gMode = 0;
//						_once = 1;
//						MessageBox(NULL, TEXT("complted"), TEXT("complted"), MB_ICONINFORMATION);
//					}
//				}
//			}
//			else {
//				if (_once == 0) {
//					gMode = 0;
//					_once = 1;
//					MessageBox(NULL, TEXT("complted"), TEXT("complted"), MB_ICONINFORMATION);
//				}
//
//			}
//		}
//
//	}
//	
//	if (len == 0x68 && (*(pp + 58) == 0x34 && *(pp + 59) == 0x01)) {
//		FFProcess& fp = FFProcess::get_instance();
//		fp.Boogiepop_GetFurnitureListFromMemory(vfurins, LOADHOUSE_P1);
//		//std::cout << "send: len = " << len << std::endl;
//		for (int i = 0; i < len; i++) {
//			std::cout << std::hex << (int)*(pp + i) << " ";
//			if ((i + 1) % 10 == 0) std::cout << std::endl;
//
//		}std::cout << std::endl;
//		std::cout << "doi:" << std::endl;
//		std::cout << "+80 +81 " << hex << (int)*(pp + 80) << " " << (int)*(pp + 81) << std::endl;
//		std::cout << "+84 +85 +86 +87" << " " << hex << (int)*(pp + 84 + 0)
//			<< " " << hex << (int)*(pp + 84 + 1)
//			<< " " << hex << (int)*(pp + 84 + 2)
//			<< " " << hex << (int)*(pp + 84 + 3) << std::endl;
//
//		std::cout << "+96 +97 +98 +99" << " " << hex << (int)*(pp + 96 + 0)
//			<< " " << hex << (int)*(pp + 96 + 1)
//			<< " " << hex << (int)*(pp + 96 + 2)
//			<< " " << hex << (int)*(pp + 96 + 3) << std::endl;
//
//		std::cout << "======" << std::endl;
//	}
//	
//	int ret = g_trueSend(s, buf, len, flags);
//	return ret;
//}

static int WINAPI hook_recv(SOCKET s, char* buf, int len, int flags)
{
	//Triger
	// cout << "recv: " << gInit << endl;
	//if (gInit == 1) {
	//	cout << "Trigger CHECK: " << gInit << endl;
	//	gInit = 0;
	//	FFProcess& fp = FFProcess::get_instance();
	//	fp.out_test();
	//}

	int ret = g_trueRecv(s, buf, len, flags);
	if (ret > 0)
	{
		char* temp = new char[ret];
		memcpy_s(temp, ret, buf, ret);
		if (_msend != NULL)
			_mrecv(temp, ret);
		delete temp;
	}
	return ret;
}
#pragma endregion

#pragma region OperateLoadhouse
static void WINAPI hook_Operate(__int64* housing, __int64* item) {
	Log() << "operator:" << housing << ", " << item << endl;
	OPITEM_P1 = (__int64)housing;
	OPITEM_P2 = (__int64)item;
	return g_trueOperate(housing, item);
}

void LOAD() {
	// -color
	FFProcess& fp = FFProcess::get_instance();
	memset(inputhousing, 0, 9600);
	vfurins.clear();
	std::vector<CategoryInstance> cateList = fp.Boogiepop_Preview_ReadCateList();
	//convert --> 24byte*100  
	//400*24bytes --> [0-99 100-199 200-299 300-399]
	//char inputhousing[9600] = { 0 };
	//start : num*24
	std::vector<int> ignoreCategoryID{ 10, 19, 22, 23, 24, 37, 38, 39, 43, 44, 45, 112, 113, 114, 207, 210, 211, 212, 213, 214, 219, 220, 221, 222, 224, 225, 226, 227, 228, 229, 236, 238, 239, 240, 241, 249, 250, 251, 252, 253, 270, 271, 285, 289, 293, 301, 302, 306, 313, 331, 344, 345, 346, 355, 358, 360, 361, 362, 363, 364, 365, 366, 367, 369, 370, 381, 382, 383, 384, 385, 386, 391, 392, 393, 394, 403, 407, 408, 410, 413, 414, 415, 423, 425, 426, 427, 438, 439, 443, 444, 445, 446, 448, 450, 453, 454, 456, 459, 462, 465, 469, 472, 473, 474, 475, 476, 481, 489, 491, 492, 500, 513, 514, 515, 516, 520, 528, 540, 541, 542, 543, 544, 546, 550, 552, 554, 557, 559, 560, 567, 568, 571, 610, 612, 613, 616, 617, 618, 619, 620, 628, 629, 630, 633, 640, 643, 644, 673, 674, 675, 676, 677, 678, 682, 683, 688, 689, 693, 694, 702, 730, 731, 741, 744, 749, 750, 751, 757, 759, 760, 761, 762, 763, 764, 775, 778, 784, 787, 790, 813, 815, 816, 817, 820, 823, 834, 835, 848, 850, 852, 853, 888, 892, 893, 917, 919, 929, 937, 942, 957, 958, 959, 960, 962, 975, 984, 985, 1026, 1027, 1029, 1030, 1031, 1032, 1037, 1042, 1058, 1063, 1078, 1118, 1121, 1127, 1128, 1129, 1131, 1147, 1153, 1157, 1159, 1165, 1168, 1174, 1188, 1201, 1220, 1248, 1249, 1276, 1278, 1279 , 1282, 1288, 1315 };
	int num = 0;
	for (int i = 0; i < cateList.size(); i++) {
		//int category = jitemlist["list"][i]["categoryId"].GetInt();
		int category = cateList[i].categoryId;
		if (std::count(ignoreCategoryID.begin(), ignoreCategoryID.end(), category)) {
			continue;
		}
		//int count = jitemlist["list"][i]["count"].GetInt();
		int count = cateList[i].count;
		for (int j = 0; j < count; j++) {
			int color = (int)cateList[i].color[j];
			float r = (float)cateList[i].r[j];
			float x = (float)cateList[i].posX[j];
			float y = (float)cateList[i].posY[j];
			float z = (float)cateList[i].posZ[j];
			//cid rxyz
			//start : num*24
			short cid = (short)category;
			unsigned char* _cid = (unsigned char*)&cid;
			for (int i = 0; i < 2; i++) {
				inputhousing[num * 24 + i] = *(_cid + i);
			}
			inputhousing[num * 24 + 2] = 0x1;
			inputhousing[num * 24 + 2+2] = (char)color;
			unsigned char* _r = (unsigned char*)&r;
			unsigned char* _px = (unsigned char*)&x;
			unsigned char* _py = (unsigned char*)&y;
			unsigned char* _pz = (unsigned char*)&z;
			for (int _b = 0; _b < 4; _b++) {
				inputhousing[num * 24 + 8 + _b] = *(_r + _b);
				inputhousing[num * 24 + 12 + _b] = *(_px + _b);
				inputhousing[num * 24 + 12 + 4 + _b] = *(_py + _b);
				inputhousing[num * 24 + 12 + 4 + 4 + _b] = *(_pz + _b);
			}
			num++;
		}
	}
	if (num > 0 && num <= 100) {
		load_num = 1;
	}
	else if (num > 100 && num <= 200) {
		load_num = 2;
	}
	else if (num > 200 && num <= 300) {
		load_num = 3;
	}
	else if (num > 300 && num <= 400) {
		load_num = 4;
	}
	else {
		load_num = 0;
	}
	total_load_num = load_num;
}
static void WINAPI hook_Loadhouse(__int64* housing, __int64* item) {
	LOADHOUSE_P1 = (__int64)housing;
	LOADHOUSE_P2 = (__int64)item;
	char temp[2416] = { 0 };
	memcpy(temp, (void*)LOADHOUSE_P2, 12);
	if (start_load == 1) {
		//set inputhouse and load_num
		if (read_json_once) {
			LOAD();
			read_json_once = 0;
		}
		if (load_num > 0) {
			int t = (total_load_num - load_num) * 2400;
			std::cout << "load:" << load_num << "  t: " << t << std::endl;
			load_num--;
			if (load_num == 0) {
				start_load = 0;
				read_json_once = 1;
			}
			memcpy(temp + 12, inputhousing + t, 2400);
			memcpy((void*)LOADHOUSE_P2, temp, 2416);
			return g_trueLoadhouse(housing, item);
		}
	}
	return g_trueLoadhouse(housing, item);
}
#pragma endregion

#pragma region Guizmo
//ImGUI imports
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <ImGuizmo.h>
//DX11 imports
#include <d3d11.h>
#include <D3Dcompiler.h>
#pragma comment(lib, "D3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")
#include <winerror.h>
// D3X HOOK DEFINITIONS
typedef HRESULT(__fastcall* IDXGISwapChainPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
// Definition of WndProc Hook. Its here to avoid dragging dependencies on <windows.h> types.
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL g_bInitialised = false;
bool g_ShowMenu = false;
// Main D3D11 Objects
ID3D11DeviceContext* pContext = NULL;
ID3D11Device* pDevice = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
static IDXGISwapChain* pSwapChain = NULL;
static WNDPROC OriginalWndProcHandler = nullptr;
HWND window = nullptr;
IDXGISwapChainPresent fnIDXGISwapChainPresent;
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
void ConsoleSetup()
{
	// With this trick we'll be able to print content to the console, and if we have luck we could get information printed by the game.
	AllocConsole();
	SetConsoleTitle(_T("[+] Hooking Console"));
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
	std::cout << "Hooking Console" << std::endl;
}
enum IDXGISwapChainvTable //for dx10 / dx11
{
	QUERY_INTERFACE,
	ADD_REF,
	RELEASE,
	SET_PRIVATE_DATA,
	SET_PRIVATE_DATA_INTERFACE,
	GET_PRIVATE_DATA,
	GET_PARENT,
	GET_DEVICE,
	PRESENT,
	GET_BUFFER,
	SET_FULLSCREEN_STATE,
	GET_FULLSCREEN_STATE,
	GET_DESC,
	RESIZE_BUFFERS,
	RESIZE_TARGET,
	GET_CONTAINING_OUTPUT,
	GET_FRAME_STATISTICS,
	GET_LAST_PRESENT_COUNT
};
void detourDirectX();
DWORD WINAPI SetupDX11Hook(LPVOID lp){
//void SetupDX11Hook() {
	Log() << "SetupDX11Hook:" << std::endl;
	
	// To create a device, device context and swap chain, only one thing is needed, which is the hWnd of the target window
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC scd{};
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = FindWindow(L"FFXIVGAME", NULL);//fp.window; // globals::hGame;
	Log() << "window: " + std::to_string((SIZE_T)scd.OutputWindow) << std::endl;
	scd.SampleDesc.Count = 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = TRUE;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;

	//https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
	//if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &feature_level, 1, D3D11_SDK_VERSION, &scd, &swapchain, &device, NULL, &context)))
	//{
	//	std::cout << "failed to create device\n";
	//	return;
	//}
	
	IDXGISwapChain* swapchain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11RenderTargetView* render_view = nullptr;
	void** pVTableSwapChain = nullptr;
	while (true) {
		HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &feature_level, 1, D3D11_SDK_VERSION, &scd, &swapchain, &device, NULL, &context);
		Log() << "result:" + std::to_string(result) << std::endl;
		// Take the value of the first-level pointer and get the IDXGISwapChain interface
		// https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nn-dxgi-idxgiswapchain
		
		Log() << "swapchain: " << swapchain << std::endl;
		Log() << "device: " << device << std::endl;
		Log() << "context: " << context << std::endl;
		if (swapchain == nullptr || device == nullptr || context == nullptr) {
			continue;
		}
		pVTableSwapChain = *reinterpret_cast<void***>(swapchain);
		break;
	}
	

	// Obtain the addresses of the two functions that need hooks, which are the two functions provided by the IDXGISwapChain interface。

	// Present the rendered image to the user. IDXGISwapChain::Present
	PVOID pPresentAddress = reinterpret_cast<LPVOID>(pVTableSwapChain[IDXGISwapChainvTable::PRESENT]);
	Log() << "SetupDX11Hook: Present: " + std::to_string((SIZE_T)pPresentAddress) << std::endl;

	fnIDXGISwapChainPresent = (IDXGISwapChainPresent)(pPresentAddress);//0x1fe0 //0x4580);//0x59e0);
	detourDirectX();
	//Mhook_SetHook((LPVOID*)&fnIDXGISwapChainPresent, Present);
	//printValues();
	return 1;
}
void retrieveValues()
{
	DWORD_PTR hDxgi = (DWORD_PTR)GetModuleHandle(_T("dxgi.dll"));
	DWORD present_offset = 0x15e0;
	Log() << "hDxgi: " + std::to_string(hDxgi) << std::endl;
	Log() << "retrieveValues:" + std::to_string(present_offset) << std::endl;
	fnIDXGISwapChainPresent = (IDXGISwapChainPresent)((DWORD_PTR)hDxgi + present_offset);//0x1fe0 //0x4580);//0x59e0);
	
	//this.Present = scanner.ScanModule("41 8B F0 8B FA 89 54 24 ?? 48 8B D9 48 89 4D ?? C6 44 24 ?? 00") - 0x37;


	//FFProcess& fp = FFProcess::get_instance();
	//fnIDXGISwapChainPresent = (IDXGISwapChainPresent)(fp.get_dxgi_present());

	
}
HRESULT GetDeviceAndCtxFromSwapchain(IDXGISwapChain* pSwapChain, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	HRESULT ret = pSwapChain->GetDevice(__uuidof(ID3D11Device), (PVOID*)ppDevice);

	if (SUCCEEDED(ret))
		(*ppDevice)->GetImmediateContext(ppContext);

	return ret;
}
LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	ImGuiIO& io = ImGui::GetIO();
	POINT mPos;
	GetCursorPos(&mPos);
	ScreenToClient(window, &mPos);
	ImGui::GetIO().MousePos.x = mPos.x;
	ImGui::GetIO().MousePos.y = mPos.y;

	if (uMsg == WM_KEYUP)
	{
		if (wParam == VK_DELETE)
		{
			g_ShowMenu = !g_ShowMenu;
		}

	}

	if (io.WantCaptureMouse)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true;
	}

	return CallWindowProc(OriginalWndProcHandler, hWnd, uMsg, wParam, lParam);
}
float matrixTranslation[3] = { 1.0f,1.0f,1.0f }, matrixRotation[3] = { 1.0f,1.0f,1.0f }, matrixScale[3] = { 0.0f,0.0f,0.0f };
float viewProjectionMatrix[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f },
	identityMatrix[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f },
	itemMatrix[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f };
std::vector<float> pos{ 0.,0.,0. };
std::vector<float> rotation{ 0.,0.,0. };
float snap[3] = { guizmoSnap, guizmoSnap, guizmoSnap };
HRESULT __fastcall Present(IDXGISwapChain* pChain, UINT SyncInterval, UINT Flags)
{
	if (!g_bInitialised) {
		Log() << "\t[+] Present Hook called by first time" << std::endl;
		if (FAILED(GetDeviceAndCtxFromSwapchain(pChain, &pDevice, &pContext)))
			return fnIDXGISwapChainPresent(pChain, SyncInterval, Flags);
		pSwapChain = pChain;
		DXGI_SWAP_CHAIN_DESC sd;
		pChain->GetDesc(&sd);
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		window = sd.OutputWindow;
		if (window == NULL) {
			Log(LogType::error) << "window=NULL" << std::endl;
			return NULL;
		}
		//Set OriginalWndProcHandler to the Address of the Original WndProc function
		OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)hWndProc);

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);
		ImGui::GetIO().ImeWindowHandle = window;

		ID3D11Texture2D* pBackBuffer;

		pChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
		pBackBuffer->Release();

		g_bInitialised = true;
	}
	/**/
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();

	ImGui::NewFrame();
	FFProcess& fp = FFProcess::get_instance();
	if (fp.CanEdit() && (!fp.isRotating())&&guizmoFlag)
	{
		ImGuiIO& io = ImGui::GetIO();
		pos = fp.GetSelectedItemPos();
		rotation = fp.GetSelectedItemRotation();
		ImGuizmo::RecomposeMatrixFromComponents(&pos[0], &rotation[0], &matrixScale[0], &itemMatrix[0]);
		for (__int64 i = 0; i < 16; i++) {
			float ff = Memory::ReadGameMemory<float>(fp.hProcess, MatrixAddr + (i * (__int64)0x4));
			viewProjectionMatrix[i] = ff;
		}
		ImGuizmo::Enable(true);
		ImGuizmo::BeginFrame();
		ImGuizmo::SetOrthographic(false);
		ImVec2 vp = ImGui::MGetViewport();
		ImGui::SetNextWindowSize(ImVec2(vp.x, vp.y));
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImVec2 v = ImVec2(-1, -1);
		bool open = false;
		ImGui::Begin("Gizmo", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		snap[0]=guizmoSnap; snap[1] = guizmoSnap; snap[2] = guizmoSnap;
		if (guizmoSnapFlag) {
			ImGuizmo::Manipulate(&viewProjectionMatrix[0], &identityMatrix[0], ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, &itemMatrix[0], NULL, &snap[0], NULL, NULL);
		}
		else {
			ImGuizmo::Manipulate(&viewProjectionMatrix[0], &identityMatrix[0], ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, &itemMatrix[0], NULL, NULL, NULL, NULL);
		}
		ImGuizmo::DecomposeMatrixToComponents(&itemMatrix[0], &pos[0], &rotation[0], &matrixScale[0]);
		fp.SetSelectedItemPos(pos);
		ImGui::EndChild();
		ImGui::End();
		ImGuizmo::SetID(-1);
	}
	ImGui::EndFrame();
	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return fnIDXGISwapChainPresent(pChain, SyncInterval, Flags);
}
void detourDirectX()
{
	Mhook_SetHook((LPVOID*)&fnIDXGISwapChainPresent, Present);
}
void printValues()
{
	Log() << "[+] ID3D11DeviceContext Addr: " << std::hex << pContext << std::endl;
	Log() << "[+] ID3D11Device Addr: " << std::hex << pDevice << std::endl;
	Log() << "[+] ID3D11RenderTargetView Addr: " << std::hex << mainRenderTargetView << std::endl;
	Log() << "[+] IDXGISwapChain Addr: " << std::hex << pSwapChain << std::endl;
}
#pragma endregion

DWORD WINAPI Check(LPVOID lp)
{
	while (1) {
		Sleep(100);
		if (gInit == 1) {
			Log() << "Trigger CHECK: " << gInit << endl;
			gInit = 0;
			FFProcess& fp = FFProcess::get_instance();
			fp.import();
		}
	}
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:{
		//ConsoleSetup();
		
		FFProcess& fp = FFProcess::get_instance();
		g_trueOperate = (_house)(fp.GetBaseAdd() + OffsetMgr::offset_OperateItem);
		g_trueLoadhouse = (_house)(fp.GetBaseAdd() + OffsetMgr::offset_Loadhouse);
		g_trueMatrix = (_matrix)(fp.GetBaseAdd() + OffsetMgr::offset_ViewMatrix);
		g_trueSavepreview = (_savepreview)(fp.GetBaseAdd() + OffsetMgr::offset_SavePreview);
		g_true_select = (_select)(fp.GetBaseAdd() + OffsetMgr::offset_Select);
		//fp.set_select_nop();
		GetModuleFileName(NULL, pCallFileName, MAX_PATH);
		Log() << "[BG] dll inject begin" << std::endl;
		Log() << "[BG] pCallFileName: " << pCallFileName << std::endl;
		if (0 == strcmp(callName_ff, TCHAR2char(pCallFileName) + strlen(TCHAR2char(pCallFileName)) - strlen(callName_ff))) {
			Log() << "[BG] dll inject end:" << callName_ff << " " << TCHAR2char(pCallFileName)<<  " --" << TCHAR2char(pCallFileName) + strlen(TCHAR2char(pCallFileName)) - strlen(callName_ff) << std::endl;
			CreateThread(
				NULL,// The security attribute of a thread kernel object
				NULL,// The size of the thread stack space
				SetupDX11Hook,// The address of the thread function executed by the new thread
				NULL,// function parameters
				0,// execute immediately
				nullptr// (out) the ID number of the thread
			);
			//Mhook_SetHook((LPVOID*)&g_trueSend, hook_send);
			//Mhook_SetHook((LPVOID*)&g_trueRecv, hook_recv);

			//Mhook_SetHook((LPVOID*)&o_WSASend, hook_WSASend);
			//Mhook_SetHook((LPVOID*)&o_WSARecv, hook_WSARecv);

			//Mhook_SetHook((LPVOID*)&g_trueOperate, hook_Operate);
			Mhook_SetHook((LPVOID*)&g_trueLoadhouse, hook_Loadhouse);

			Log() << "[BG] before hook_matrix" << std::endl;
			Mhook_SetHook((LPVOID*)&g_trueMatrix, hook_matrix);
			Log() << "[BG] end hook_matrix" << std::endl;

			Mhook_SetHook((LPVOID*)&g_trueSavepreview, hook_savepreview);

			
			//Mhook_SetHook((LPVOID*)&g_true_select, hook_select);
			
			//Mhook_SetHook((LPVOID*)&g_trueOperate, hook_Operate);
			//retrieveValues();
			//detourDirectX();
			//printValues();
			DWORD threadid;
			HANDLE hthread = CreateThread(
				NULL, // The security attribute of a thread kernel object
				NULL, // The size of the thread stack space
				Check, // The address of the thread function executed by the new thread
				NULL, // function parameters
				0, // function parameters
				&threadid // (out) the ID number of the thread
			);
			Log() << "[BG] Create Thread:" << hthread << endl;
		}
		break; }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		Mhook_Unhook((LPVOID*)&g_trueSend);
		Mhook_Unhook((LPVOID*)&g_trueRecv);
		Mhook_Unhook((LPVOID*)&g_trueOperate);
		Mhook_Unhook((LPVOID*)&g_trueLoadhouse);
        break;
    }
    return TRUE;
}

