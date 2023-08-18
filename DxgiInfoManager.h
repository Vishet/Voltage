#pragma once
#include "VoltageWindows.h"
#include <vector>
#include <string>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;

	~DxgiInfoManager();

	void Set();
	std::vector<std::string> GetMessages() const;

private:
	unsigned long long next = 0u;
	struct IDXGIInfoQueue* dxgiInfoQueue = nullptr;
};

