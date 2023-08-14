// --------------------------------------------
// Application.h
// Application.cpp
// --------------------------------------------
// The Application class creates the main
// render window, handles processing
// events, and updates and draws whichever
// test is currently active. All of the
// ImGui menus are handled here also.
// --------------------------------------------

#ifndef APPLICATION_H
#define APPLICATION_H

#include "imgui.h"
#include "imgui-SFML.h"
#include "Raytracer.h"
#include "AStar.h"

#include <SFML/Graphics.hpp>

enum class TestID
{
	T01_RAYTRACER = 0,
	T02_PATHFINDING
};

class Application
{
public:
	Application();
	~Application();
	void start();

private:
	static const unsigned int SCREEN_WIDTH = 1280u;
	static const unsigned int SCREEN_HEIGHT = 720u;
	sf::RenderWindow window;
	bool exitApp;
	Raytracer *raytracer = nullptr;

	void processEvents();
	void update(const sf::Time &dt);
	void handleUI();
	void loadTest(TestID testID);
	void draw();
};

#endif // !APPLICATION_H