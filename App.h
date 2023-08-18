#pragma once
#include "Window.h"
#include "Timer.h"

class App
{
private:
	Window window;
	Timer timer;

public:
	App();

	int Start();

private:
	void UpdateFrame();
};

