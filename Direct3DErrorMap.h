#pragma once
#include "VoltageWindows.h"
#include <unordered_map>
#include <string>

class Direct3DErrorMap
{
public:
	Direct3DErrorMap() = delete;
	static std::string GetErrorString(HRESULT hr);

private:
	static std::unordered_map<HRESULT, std::string> map;
};

