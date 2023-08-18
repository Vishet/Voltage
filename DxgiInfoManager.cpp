#include "DxgiInfoManager.h"
#include "Graphics.h"
#include "Window.h"
#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = hrcall ) ) throw Graphics::HrException(__LINE__, __FILE__, hr)

DxgiInfoManager::DxgiInfoManager()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	const auto modDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (modDxgiDebug == nullptr) throw VWND_LAST_EXCEPT();

	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void**>(GetProcAddress(modDxgiDebug, "DXGIGetDebugInterface"))
	);
	if (DxgiGetDebugInterface == nullptr) throw VWND_LAST_EXCEPT();

	HRESULT hr;
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&dxgiInfoQueue)));
}

DxgiInfoManager::~DxgiInfoManager()
{
	if (dxgiInfoQueue != nullptr) dxgiInfoQueue->Release();
}

void DxgiInfoManager::Set()
{
	next = dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength = 0;

		GFX_THROW_NOINFO(dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));

		auto bytes = std::make_unique<byte[]>(messageLength);
		auto message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		GFX_THROW_NOINFO(dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, message, &messageLength));
		messages.emplace_back(message->pDescription);
	}

	return messages;
}
