#pragma once
#include <WinUser.h>
#include <windef.h>

void send_click(HWND hWnd) {
	SetForegroundWindow(hWnd);
	RECT window_rect;
	GetWindowRect(hWnd, &window_rect);
	LONG px = (window_rect.left + window_rect.right) / 2;
	LONG py = (window_rect.top + window_rect.bottom) / 2;

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, SPIF_SENDCHANGE);
	int client_width = (rect.right - rect.left);
	int client_height = (rect.bottom - rect.top);

	px = 65535 * ((double)px / client_width);
	py = 65535 * ((double)py / client_height);
	INPUT in[2]; // 0 = left dn, 1 = left up
	ZeroMemory(in, sizeof(INPUT) * 2);

	in[0].mi.dx = px;
	in[0].mi.dy = py;
	in[0].type = INPUT_MOUSE;
	in[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE;

	in[1].mi.dx = px;
	in[1].mi.dy = py;
	in[1].type = INPUT_MOUSE;
	in[1].mi.dwFlags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE;

	SendInput(2, in, sizeof(INPUT));

}