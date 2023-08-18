#pragma once
#include "VoltageWindows.h"
#include "VoltageException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <optional>

class Window
{
public:
	class Exception : public VoltageException {
	private:
		const HRESULT errorCode;

	public:
		Exception(int line, const char* file, HRESULT errorCode);
		virtual const char* what() const override;
		virtual const char* GetType() const override;
		HRESULT GetErrorCode() const;
		std::string GetErrorString() const;
		static std::string TranslateErrorCode(HRESULT errorCode);
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

public:
	Window(int width, int height, const char* title);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	static std::optional<int> ProcessMessages();

	void SetTitle(const char* title);

private:
	static LRESULT WINAPI HandleMessageSetup(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMessageThunk(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
	bool HandleMouseCapture(int x, int y);

public:
	Keyboard keyboard;
	Mouse mouse;
};

#define VWND_EXCEPT(code) Window::Exception(__LINE__, __FILE__, code)
#define VWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())

