#include "Application.h"

/// <summary>
/// Application constructor.
/// </summary>
Application::Application() : window{ sf::VideoMode{ SCREEN_WIDTH, SCREEN_HEIGHT, 32 }, "ST vs MT Perf Tests", sf::Style::Default }
{
	exitApp = false;
}

/// <summary>
/// Application destructor.
/// </summary>
Application::~Application()
{

}

/// <summary>
/// Start.
/// </summary>
void Application::start()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time timePerFrame = sf::seconds(1.f / 60.0f);

	while (window.isOpen() && !exitApp)
	{
		timeSinceLastUpdate += clock.restart();

		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame);
		}		

		draw();
	}
}
/// <summary>
/// Process events.
/// </summary>
void Application::processEvents()
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}

		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				exitApp = true;
			}
		}
	}
}

/// <summary>
/// Update.
/// </summary>
/// <param name="dt">Delta time.</param>
void Application::update(const sf::Time &dt)
{

}

/// <summary>
/// Draw.
/// </summary>
void Application::draw()
{
	window.clear(sf::Color::Black);

	window.display();
}