#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
	:
	wnd(800, 600, "The Donkey fart box")
{}

int App::Go()
{
	while (true)
	{
		// process all messages pending, but to not block
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're return exit code
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f; // random color value generation for buffer texture clearing
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	float angle = timer.Peek(); // используем time.Peek чтобы увеличивать значения angle со временем
	float mouseX = (wnd.mouse.GetPosX() / 400.0f) - 1.0f;
	float mouseY = -((wnd.mouse.GetPosY() / 300.0f) - 1.0f);
	wnd.Gfx().DrawTestTriangle(
		angle,
		mouseX,
		mouseY
	);
	wnd.Gfx().EndFrame();
	// contain the top level of our game logic
	//const float t = timer.Peek();
	//std::ostringstream oss;
	//oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << "s";
	//wnd.SetTitle(oss.str());
}
