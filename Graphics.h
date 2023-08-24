#pragma once
#include "VoltageWindows.h"
#include "VoltageException.h"
#include "DxgiInfoManager.h"
#include <d3d11.h>
#include <wrl.h>

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

	class InfoException : public Exception
	{
	private:
		std::string info;

	public:
		InfoException(int line, const char* file, const std::vector<std::string>& infos);
		const char* what() const override;
		const char* GetType() const override;
		const std::string& GetErrorInfo() const;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr <IDXGISwapChain> swap;
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> targetView;

#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif // !NDEBUG


public:
	Graphics(HWND outputWindow);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;

	void PresentFrame();

	void ClearBuffer(float red, float green, float blue);

	void DrawTestTriangle();
};

