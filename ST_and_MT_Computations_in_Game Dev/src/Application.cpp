#include "Application.h"

/// <summary>
/// Application constructor.
/// </summary>
Application::Application() : window{ sf::VideoMode{ SCREEN_WIDTH, SCREEN_HEIGHT, 32 }, "ST vs MT Perf Tests", sf::Style::Default }
{
	exitApp = false;

	threadPool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency());

	ImGui::SFML::Init(window);
}

/// <summary>
/// Application destructor.
/// </summary>
Application::~Application()
{
	ImGui::SFML::Shutdown();
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
			draw();
		}
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
		ImGui::SFML::ProcessEvent(window, event);

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
	ImGui::SFML::Update(window, dt);

	// Test ImGui window
	ImGui::Begin("Project");
	ImGui::Text("Test Threadpool");

	if (ImGui::Button("Single Threaded", ImVec2(120, 24)))
	{
		executeTest(Test::SINGLE_THREADED);
	}

	if (ImGui::Button("Multi Threaded", ImVec2(120, 24)))
	{
		executeTest(Test::MULTI_THREADED);
	}

	ImGui::End();
}

/// <summary>
/// Draw.
/// </summary>
void Application::draw()
{
	window.clear(sf::Color::Black);

	ImGui::SFML::Render(window);

	window.display();
}

/// <summary>
/// Execute the threadpool test.
/// </summary>
void Application::executeTest(Test test)
{
	switch (test)
	{
	case Test::SINGLE_THREADED:

		// Execute jobs one after another
		for (int i = 0; i < 1000; i++)
		{
			job();
		}

		break;

	case Test::MULTI_THREADED:

		// Add jobs to the threadpool
		for (int i = 0; i < 1000; i++)
		{
			threadPool->addJob([this] { job(); });
		}

		break;
	}
}

/// <summary>
/// This function is used to test that the threadpool is working.
/// </summary>
void Application::job()
{	
	std::cout << "Thread " << std::this_thread::get_id() << " finished" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
