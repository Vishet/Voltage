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
	window.Gfx().ClearBuffer(0.0f, 0.0f, 0.0f);
	window.Gfx().DrawTestTriangle();
	window.Gfx().PresentFrame();
}
