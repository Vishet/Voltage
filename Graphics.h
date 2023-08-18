#pragma once
#include "VoltageWindows.h"
#include <d3d11.h>

class Graphics
{
private:
	ID3D11Device* device = nullptr;
	IDXGISwapChain* swap = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11RenderTargetView* targetView = nullptr;

public:
	Graphics(HWND outputWindow);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	~Graphics();

	void PresentFrame();

	void ClearBuffer(float red, float green, float blue);
};

