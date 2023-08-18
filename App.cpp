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
	const float elapsedTime = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision(2) << std::fixed << elapsedTime << " seconds";
	window.SetTitle(oss.str().c_str());
}
