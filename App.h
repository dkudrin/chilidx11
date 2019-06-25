#pragma once
#include "Window.h"
#include "ChiliTimer.h"

class App
{
public:
	App();
	// master frame / message loop
	int Go(); // start game loop
private:
	void DoFrame(); // calling every frame
private:
	Window wnd; // приложение включает в себя объект окна
	ChiliTimer timer;
};

