#include "ParticleEffect.h"

/// <summary>
/// ParticleEffect constructor.
/// </summary>
ParticleEffect::ParticleEffect(sf::Time &dt) : dt(dt)
{
	// This is where the particles will be rendered
	renderTexture = std::make_unique<sf::Texture>();
	renderTexture->create(1280, 720);

	// Pixel buffer
	pixels.resize(1280 * 720 * 4); // RGBA
	pixelsPtr = &pixels;
	scrW = 1280;

	threadAmount = std::thread::hardware_concurrency() - 1; // Subtract 1 to leave a spare thread

	threadPool = std::make_unique<ThreadPool>(threadAmount);

	// Amount of thread gen is equal to the amount of available threads
	threadGens.resize(threadAmount);

	// Each thread has its own particle generator
	int maxParticlesPerThread = maxNumOfParticles / threadAmount;

	for (int i = 0; i < threadGens.size(); ++i)
	{
		threadGens.at(i) = new Generator(startPosition, this->dt, numOfParticles, speed, timeToLive, pixelsPtr);

		for (int n = 0; n < maxParticlesPerThread; ++n)
		{
			threadGens.at(i)->particlePool.push_back(new DefaultParticle(gen));
		}
	}
}

/// <summary>
/// ParticleEffect destructor.
/// </summary>
ParticleEffect::~ParticleEffect()
{

}


/// <summary>
/// Update.
/// </summary>
/// <param name="dt">Delta time.</param>
void ParticleEffect::update(const sf::Time &dt)
{
	clTimer += clock.restart();

	// Set every value to 0
	std::fill(pixels.begin(), pixels.end(), 0);

	// Store futures in here
	std::vector<std::future<void>> futures;

	Timer timer("Particle Effect Update");

	// Update particles
	if (!multiThreaded)
	{
		for (int i = 0; i < threadGens.size(); ++i)
		{
			// Single threaded
			threadGens.at(i)->update(scrW, threadAmount);
		}

		if (clTimer > sf::seconds(0.5f))
		{
			ms = timer.stop();
			clTimer = sf::Time::Zero;
		}
	}
	else
	{
		for (int i = 0; i < threadGens.size(); ++i)
		{
			// Multi threaded
			auto f = threadPool->addJob([=]
				{
					threadGens.at(i)->update(scrW, threadAmount);
				});

			futures.push_back(std::move(f));
		}
	}

	if (multiThreaded)
	{
		// Wait for all threads to finish
		for (auto &future : futures)
		{
			future.wait();
		}

		if (clTimer > sf::seconds(0.5f))
		{
			ms = timer.stop();
			clTimer = sf::Time::Zero;
		}
	}

	renderTexture->update(pixels.data());
}

/// <summary>
/// Update and draw all ImGui menus.
/// </summary>
void ParticleEffect::handleUI()
{
	ImGui::PushItemWidth(0); 

	if (ImGui::CollapsingHeader("Particle Effect##006"))
	{
		// Instructions
		if (ImGui::CollapsingHeader("Instructions##007"))
		{
			ImGui::Dummy(ImVec2(0.0f, 8.0f));

			ImGui::Text("Use the mouse to move the particles");
			ImGui::Text("around on the screen. Some strange");
			ImGui::Text("behaviour can be seen sometimes when");
			ImGui::Text("modifying the particle count, but this");
			ImGui::Text("stops once the particles in the pool");
			ImGui::Text("have been fully reset.");

			ImGui::Dummy(ImVec2(0.0f, 8.0f));
		}

		// Particle properties
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Number of Particles##008");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::InputInt("Amount##009", &numOfParticles);	

		numOfParticles = std::clamp(numOfParticles, 500, 2000000);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Particle Attributes##010");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SliderFloat("Speed##011", &speed, 10.0f, 500.0f);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SliderFloat("Lifetime##012", &timeToLive, 0.5f, 100.0f);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Window Coordinates##013");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		std::string strX = "X: " + std::to_string(startPosition.x);
		std::string strY = "Y: " + std::to_string(startPosition.y);

		ImGui::Text(strX.c_str());
		ImGui::Text(strY.c_str());

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		if (ImGui::CollapsingHeader("Threads##014"))
		{
			ImGui::Dummy(ImVec2(0.0f, 8.0f));

			ImGui::SeparatorText("Thread Usage##015");

			ImGui::Dummy(ImVec2(0.0f, 8.0f));

			static int sel_sd6 = 0;
			ImGui::RadioButton("Single-threaded##016", &sel_sd6, 0);
			ImGui::RadioButton("Multi-threaded##017", &sel_sd6, 1);
			sel_sd6 == 0 ? multiThreaded = false : multiThreaded = true;

			ImGui::Dummy(ImVec2(0.0f, 8.0f));

			ImGui::SeparatorText("Particle Update Time##018");

			ImGui::Dummy(ImVec2(0.0f, 8.0f));

			std::string milliSecs = "Frame Time: " + std::to_string(ms) + "ms";

			ImGui::Text(milliSecs.c_str());

			ImGui::Dummy(ImVec2(0.0f, 8.0f));
		}
	}

	// Render output window
	ImGui::Begin("Particle Effect##019");

	// The content region position used to map the
	// mouse position correctly
	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	vMin.x += ImGui::GetWindowPos().x;
	vMin.y += ImGui::GetWindowPos().y;
	vMax.x += ImGui::GetWindowPos().x;
	vMax.y += ImGui::GetWindowPos().y;

	// Handle ImGui window resize
	ImVec2 currentWindowSize = ImGui::GetWindowSize();

	if (currentWindowSize.x != renderWindowSize.x || currentWindowSize.y != renderWindowSize.y)
	{
		renderWindowSize = currentWindowSize;

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		float sizeX = vMax.x - vMin.x;
		float sizeY = vMax.y - vMin.y;

		renderTexture = std::make_unique<sf::Texture>();
		renderTexture->create(sizeX, sizeY);

		pixels.clear();
		pixels.resize(sizeX * sizeY * 4);
		pixelsPtr = &pixels;
		scrW = sizeX;
	}

	// Get current mouse coordinates from ImGui window
	ImVec2 mousePos = ImGui::GetMousePos();

	// As the mouse position is relative to the entire window,
	// the region content position must be taken into account
	mousePos.x -= vMin.x;
	mousePos.y -= vMin.y;

	startPosition.x = mousePos.x;
	startPosition.y = mousePos.y;

	ImGui::Image(*renderTexture);

	ImGui::End();
}
