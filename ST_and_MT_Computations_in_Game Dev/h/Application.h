#ifndef APPLICATION_H
#define APPLICATION_H

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"
#include "Vec3.h"

#include <SFML/Graphics.hpp>

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
	std::unique_ptr<ThreadPool> threadPool;

	void processEvents();
	void update(const sf::Time &dt);
	void draw();
};

#endif // !APPLICATION_H