#pragma once
#include "VoltageWindows.h"
#include "VoltageException.h"
#include "DxgiInfoManager.h"
#include <d3d11.h>

class Graphics
{
public:
	class Exception : public VoltageException
	{
		using VoltageException::VoltageException;
		
	public:
		static std::string GetErrorString(HRESULT hr);
	};

	class HrException : public Exception
	{
	private:
		HRESULT hr;
		std::string info;

	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessages = {});
		const char* what() const override;
		const char* GetType() const override;
		HRESULT GetErrorCode() const;
		std::string GetErrorDescription() const;
		std::string GetErrorInfo() const;
	};
	
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;

	private:
		std::string reason;
	public:
		const char* GetType() const override;
	};

private:
	ID3D11Device* device = nullptr;
	IDXGISwapChain* swap = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11RenderTargetView* targetView = nullptr;

#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif // !NDEBUG


public:
	Graphics(HWND outputWindow);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	~Graphics();

	void PresentFrame();

	void ClearBuffer(float red, float green, float blue);
};

