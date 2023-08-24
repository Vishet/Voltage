#include "Graphics.h"
#include "Direct3DErrorMap.h"
#include <sstream>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")

#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = hrcall ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); { auto mv = infoManager.GetMessages(); if(!mv.empty()) throw Graphics::InfoException(__LINE__, __FILE__, mv); }
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif // !NDEBUG

namespace wrl = Microsoft::WRL;

Graphics::Graphics(HWND outputWindow)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = outputWindow;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	HRESULT hr;

	GFX_THROW_INFO( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&swap,
		&device,
		nullptr,
		&context
	));

	wrl::ComPtr<ID3D11Resource> backBuffer;
	GFX_THROW_INFO( swap->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer));
	GFX_THROW_INFO( device->CreateRenderTargetView(backBuffer.Get(), nullptr, &targetView));
}

void Graphics::PresentFrame()
{
	HRESULT hr;

#ifndef NDEBUG
	infoManager.Set();
#endif // !NDEBUG


	if ( FAILED(hr = swap->Present(1u, 0u)) )
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED) throw GFX_DEVICE_REMOVED_EXCEPT(device->GetDeviceRemovedReason());
		else throw GFX_EXCEPT(hr);
	};
}

void Graphics::ClearBuffer(float red, float green, float blue)
{
	const FLOAT colors[] = { red, green, blue, 1.0f };
	context->ClearRenderTargetView(targetView.Get(), colors);
}

void Graphics::DrawTestTriangle()
{
	HRESULT hr{};
	namespace wrl = Microsoft::WRL;

	struct Vertex
	{
		float x;
		float y;
	};

	const Vertex vertices[] =
	{
		{0.0f, 0.5f},
		{0.5f, -0.5f},
		{-0.5f, -0.5f}
	};

	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(vertices);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = vertices;

	wrl::ComPtr<ID3D11Buffer> vertexBuffer;
	GFX_THROW_INFO(device->CreateBuffer(&bd, &subData, &vertexBuffer));

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);

	wrl::ComPtr<ID3D11PixelShader> pixelShader;
	wrl::ComPtr<ID3DBlob> shaderBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &shaderBlob));
	GFX_THROW_INFO(device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		nullptr,
		&pixelShader
	));
	context->PSSetShader(pixelShader.Get(), 0, 0);

	wrl::ComPtr<ID3D11VertexShader> vertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &shaderBlob));
	GFX_THROW_INFO(device->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		nullptr,
		&vertexShader
	));
	context->VSSetShader(vertexShader.Get(), 0, 0);

	wrl::ComPtr<ID3D11InputLayout> inputLayout;
	const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	GFX_THROW_INFO(device->CreateInputLayout(
		inputElementDesc,
		static_cast<UINT>(std::size(inputElementDesc)),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		&inputLayout
	));
	context->IASetInputLayout(inputLayout.Get());

	context->OMSetRenderTargets(1u, targetView.GetAddressOf(), nullptr);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT viewPort;
	viewPort.Width = 800;
	viewPort.Height = 600;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	context->RSSetViewports(1u, &viewPort);

	GFX_THROW_INFO_ONLY(context->Draw(static_cast<UINT>(std::size(vertices)), 0u));
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessages) :
	Exception(line, file),
	hr(hr)
{
	for (const auto& message : infoMessages)
	{
		info += message;
		info.push_back('\n');
	}

	if (!info.empty()) info.pop_back();
}

const char* Graphics::HrException::what() const
{
	std::ostringstream oss;
	oss << "[Error code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << static_cast<unsigned long>(GetErrorCode()) << ")" << std::endl
		<< "[Error string] " << GetErrorString(hr) << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;

	if (!info.empty()) oss << "\n[Error info]\n" << GetErrorInfo() << std::endl << std::endl;

	oss << GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const
{
	return "Voltage graphics exception";
}

HRESULT Graphics::HrException::GetErrorCode() const
{
	return hr;
}

std::string Graphics::HrException::GetErrorDescription() const
{
	char* messageBuffer = nullptr;
	DWORD msgLength = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr
	);
	if (msgLength == 0) return "Unindentified error code";
	else
	{
		std::string errorString = messageBuffer;
		LocalFree(messageBuffer);
		return errorString;
	}
}

std::string Graphics::HrException::GetErrorInfo() const
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const
{
	return "Voltage graphics exception: [Device removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

std::string Graphics::Exception::GetErrorString(HRESULT hr)
{
	return Direct3DErrorMap::GetErrorString(hr);
}

Graphics::InfoException::InfoException(int line, const char* file, const std::vector<std::string>& infos) :
	Exception(line, file),
	info()
{
	for (const auto& info : infos)
	{
		this->info += info;
		this->info.push_back('\n');
	}

	if (!this->info.empty()) this->info.pop_back();
}

const char* Graphics::InfoException::what() const
{
	std::ostringstream oss;
	oss	<< "[Information]\n" << GetErrorInfo() << std::endl << std::endl
		<< GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const
{
	return "Voltage info exception";
}

const std::string& Graphics::InfoException::GetErrorInfo() const
{
	return info;
}
