#pragma once
#include "Window.h"
#include "ChiliTimer.h"

class App
{
public:
	App();
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
private:
	Window wnd;
	ChiliTimer timer;
	std::vector<std::unique_ptr<class Box>> boxes;
};