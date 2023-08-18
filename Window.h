#pragma once
#include "VoltageWindows.h"
#include "VoltageException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>

#define VWND_EXCEPT(code) Window::HrException(__LINE__, __FILE__, code)
#define VWND_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError())
#define VWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__, __FILE__)

class Window
{
public:
	class Exception : public VoltageException
	{
		using VoltageException::VoltageException;

	public:
		static std::string TranslateErrorCode(HRESULT errorCode);
	};

	class HrException : public Exception 
	{
	private:
		const HRESULT errorCode;

	public:
		HrException(int line, const char* file, HRESULT errorCode);
		virtual const char* what() const override;
		virtual const char* GetType() const override;
		HRESULT GetErrorCode() const;
		std::string GetErrorString() const;
	};

	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const override;
	};

private:
	class WindowClass {
	private:
		HINSTANCE instance;
		static constexpr const char* name = "Voltage";
		static WindowClass windowClass;
	public:
		WindowClass();
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;

		static const char* GetName();
		static HINSTANCE GetInstance();
	};

private:
	int width;
	int height;
	HWND handle;
	std::unique_ptr<Graphics> graphics;

public:
	Window(int width, int height, const char* title);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	static std::optional<int> ProcessMessages();

	void SetTitle(const std::string& title);

	Graphics& Gfx();

private:
	static LRESULT WINAPI HandleMessageSetup(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMessageThunk(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	bool HandleMouseCapture(int x, int y);

public:
	Keyboard keyboard;
	Mouse mouse;
};

