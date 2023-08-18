#include "Window.h"
#include "resource.h"
#include "Graphics.h"

#include <sstream>



Window::WindowClass Window::WindowClass::windowClass;

Window::Exception::Exception(int line, const char* file, HRESULT errorCode) :
	VoltageException(line, file),
	errorCode(errorCode)
{

}

const char* Window::Exception::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const 
{
	return "Voltage Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT errorCode) 
{
	char* messageBuffer = nullptr;
	DWORD msgLength = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
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

HRESULT Window::Exception::GetErrorCode() const
{
	return errorCode;
}

std::string Window::Exception::GetErrorString() const 
{
	return TranslateErrorCode(errorCode);
}

Window::WindowClass::WindowClass() :
	instance(GetModuleHandle(0))
{
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = Window::HandleMessageSetup;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetInstance();
	windowClass.hIcon = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 32, 32, 0
	));
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszClassName = GetName();
	windowClass.hIconSm = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 16, 16, 0
	));;
	RegisterClassEx(&windowClass);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(GetName(), GetInstance());
}

const char* Window::WindowClass::GetName()
{
	return name;
}

HINSTANCE Window::WindowClass::GetInstance()
{
	return windowClass.instance;
}

Window::Window(int width, int height, const char* title) :
	width(width),
	height(height),
	handle(),
	keyboard(),
	mouse()
{
	RECT rect;
	rect.left = 200;
	rect.right = width + rect.left;
	rect.top = 200;
	rect.bottom = height + rect.top;

	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	if (AdjustWindowRect(&rect, style, FALSE) == 0)
		throw VWND_LAST_EXCEPT();

	handle = CreateWindow(
		WindowClass::GetName(), title, style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, WindowClass::GetInstance(), this);

	if (handle == nullptr)
		throw VWND_LAST_EXCEPT();

	graphics = std::make_unique<Graphics>(handle);

	ShowWindow(handle, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(handle);
}

std::optional<int> Window::ProcessMessages()
{
	MSG message;
	while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
		if (message.message == WM_QUIT) return static_cast<int>(message.wParam);

		TranslateMessage(&message);
		DispatchMessage(&message);
	}


	return {};
}

void Window::SetTitle(const char* title)
{
	if (SetWindowText(handle, title) == 0)
		throw VWND_LAST_EXCEPT();
}

Graphics& Window::Gfx()
{
	return *graphics;
}

LRESULT WINAPI Window::HandleMessageSetup(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE) {
		const auto createStructPointer = reinterpret_cast<const CREATESTRUCT*>(lParam);
		const auto windowPointer = static_cast<Window*>(createStructPointer->lpCreateParams);
		SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPointer));

		SetWindowLongPtr(
			windowHandle,
			GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(windowPointer->HandleMessageThunk)
		);

		return windowPointer->HandleMessage(windowHandle, message, wParam, lParam);
	}
	else
		return DefWindowProc(windowHandle, message, wParam, lParam);
}

LRESULT WINAPI Window::HandleMessageThunk(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	const auto window = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
	return window->HandleMessage(windowHandle, message, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		if(!(lParam & 0x40000000) || keyboard.IsAutorepeatEnable())
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		return 0;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		return 0;

	case WM_CHAR:
		keyboard.OnChar(static_cast<unsigned char>(wParam));
		return 0;

	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);

		if (HandleMouseCapture(pt.x, pt.y))
			mouse.OnMove(pt.x, pt.y);

		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLMBPressed();
		return 0;
	}

	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRMBPressed();
		return 0;
	}
	
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLMBReleased();
		HandleMouseCapture(pt.x, pt.y);

		return 0;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRMBReleased();
		HandleMouseCapture(pt.x, pt.y);
		
		return 0;
	}

	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheel(delta);

		return 0;
	}

	case WM_KILLFOCUS:
		keyboard.ClearState();
		return 0;

	default:
		return DefWindowProc(windowHandle, message, wParam, lParam);
	}
}

bool Window::HandleMouseCapture(int x, int y)
{
	if (x >= 0 && x < width && y >= 0 && y < height)
	{
		if (!mouse.IsInWindow())
		{
			SetCapture(handle);
			mouse.OnEnter();
		}
	}
	else if (!mouse.IsLMBPressed() && !mouse.IsRMBPressed())
	{
		ReleaseCapture();
		mouse.OnLeave();

		return false;
	}

	return true;
}
