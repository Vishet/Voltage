#include "VoltageWindows.h"
#include "App.h"
#include <sstream>

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) 
{
	try
	{
		return App().Start();
	}
	catch (const VoltageException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, e.what(), "Standard execption", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details avaliable", "Unknown exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}