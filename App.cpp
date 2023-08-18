#include "App.h"

App::App() :
	window(800, 600, "Voltage"),
	timer()
{
}

int App::Start()
{
	while (true)
	{
		if (const auto exitcode = Window::ProcessMessages())
			return *exitcode;

		UpdateFrame();
	}

	return 0;
}

void App::UpdateFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	window.Gfx().ClearBuffer(0.0f, c, c);
	window.Gfx().PresentFrame();
}
