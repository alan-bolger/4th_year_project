#include "Application.h"

/// <summary>
/// Application constructor.
/// </summary>
Application::Application() : window{ sf::VideoMode{ SCREEN_WIDTH, SCREEN_HEIGHT, 32 }, "ST vs MT Test Suite", sf::Style::Default }
{
	exitApp = false;

	// Initialise ImGui SFML
	ImGui::SFML::Init(window);

	// Enable docking
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

/// <summary>
/// Application destructor.
/// </summary>
Application::~Application()
{
	if (raytracer) { delete raytracer; }
	if (pathfinding) { delete pathfinding; }
	if (particleEffect) { delete particleEffect; }
	if (terrainGenerator) { delete terrainGenerator; }

	ImGui::SFML::Shutdown();
}

/// <summary>
/// Start.
/// </summary>
void Application::start()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

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

	// Cover entire window with dockspace
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	// Update anything that needs to be updated
	if (pathfinding != nullptr) { pathfinding->update(dt); }
	if (particleEffect != nullptr) { particleEffect->update(dt); }

	// Update and draw UI
	handleUI();
}

/// <summary>
/// Update and draw all ImGui menus.
/// </summary>
void Application::handleUI()
{
	// Select and load a test
	ImGui::Begin("Test Inspector##001");

	if (ImGui::CollapsingHeader("Load Test##002"))
	{
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::Text("Select test");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::PushItemWidth(-1);

		const char *items[] = { "T01 - Raytracing", "T02 - Pathfinding", "T03 - Particle Effect", "T04 - Terrain Generator" };
		static int item_current = 0;
		ImGui::ListBox("ListBox##004", &item_current, items, IM_ARRAYSIZE(items), 4);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		if (ImGui::Button("Load Test##005", ImVec2(100, 24)))
		{
			loadTest(static_cast<TestID>(item_current));
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 8.0f));

	if (raytracer != nullptr) {	raytracer->handleUI(); }
	if (pathfinding != nullptr) { pathfinding->handleUI(); }
	if (particleEffect != nullptr) { particleEffect->handleUI(); }
	if (terrainGenerator != nullptr) { terrainGenerator->handleUI(); }

	ImGui::End();
}

/// <summary>
/// Loads a test.
/// </summary>
/// <param name="testID">The test ID.</param>
void Application::loadTest(TestID testID)
{
	// This is a bit clunky, but sure it is what it is
	switch (testID)
	{
		case TestID::T01_RAYTRACER:
		{
			if (raytracer == nullptr)
			{
				raytracer = new Raytracer(1280, 720);
			}

			break;
		}

		case TestID::T02_PATHFINDING:
		{
			if (pathfinding == nullptr)
			{
				pathfinding = new Pathfinding();
			}

			break;
		}

		case TestID::T03_PARTICLE_EFFECT:
		{
			if (particleEffect == nullptr)
			{
				particleEffect = new ParticleEffect(timePerFrame);
			}

			break;
		}

		case TestID::T04_TERRAIN_GENERATOR:
		{
			if (terrainGenerator == nullptr)
			{
				terrainGenerator = new TerrainGenerator();
			}

			break;
		}
	}
}

/// <summary>
/// Draw.
/// </summary>
void Application::draw()
{
	window.clear(sf::Color::Black);

	ImGui::SFML::Render(window);

	if (pathfinding != nullptr) { pathfinding->render(); }

	window.display();
}
