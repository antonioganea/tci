#pragma once

#include <d3d9.h>

namespace gui {
	constexpr int WIDTH = 500;
	constexpr int HEIGHT = 500;

	extern bool exit;

	extern HWND window;
	extern WNDCLASSEXA windowClass;

	extern POINTS position;

	extern PDIRECT3D9 d3d;
	extern LPDIRECT3DDEVICE9 device;
	extern D3DPRESENT_PARAMETERS presentParameters;

	void CreateHWindow(
		const char* windowName,
		const char* className);

	void DestroyHWindow();

	bool CreateDevice();
	void ResetDevice();
	void DestroyDevice();

	void CreateImGui();
	void DestroyImGui();

	void BeginRender();
	void EndRender();
	void Render();
}