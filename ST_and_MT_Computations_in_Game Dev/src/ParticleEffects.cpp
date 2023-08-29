#include "ParticleEffects.h"

/// <summary>
/// ParticleEffects constructor.
/// </summary>
ParticleEffects::ParticleEffects(sf::Time &dt) : dt(dt)
{
	renderTexture = std::make_unique<sf::Texture>();
	renderTexture->create(1280, 720);

	pixels.resize(1280 * 720 * 4); // RGBA
	pixelsPtr = &pixels;
	scrW = 1280;

	threadPool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency() - 2);

	threadGens.resize(std::thread::hardware_concurrency() - 2);

	// Each thread has its own particle generator (in theory anyway)
	for (int i = 0; i < threadGens.size(); ++i)
	{
		threadGens.at(i) = new Generator(startPosition, this->dt, numOfParticles, pixelsPtr);

		for (int n = 0; n < maxNumOfParticles; ++n)
		{
			threadGens.at(i)->particlePool.push_back(new DefaultParticle());
		}
	}
}

/// <summary>
/// ParticleEffects destructor.
/// </summary>
ParticleEffects::~ParticleEffects()
{

}


/// <summary>
/// Update.
/// </summary>
/// <param name="dt">Delta time.</param>
void ParticleEffects::update(const sf::Time &dt)
{
	// Set every value to 0
	std::fill(pixels.begin(), pixels.end(), 0);

	// Store futures in here
	std::vector<std::future<void>> futures;

	// Update particles
	for (int i = 0; i < threadGens.size(); ++i)
	{
		if (!multiThreaded)
		{
			// Single threaded
			threadGens.at(i)->update(scrW);
		}
		else
		{
			// Multi threaded
			//auto f = threadPool->addJob([=]
			//	{
			//		threadGens.at(i)->update(scrW);
			//	});
			
			//futures.push_back(std::move(f));
		}
	}

	if (multiThreaded)
	{
		// Wait for all threads to finish
		//for (auto &future : futures)
		//{
		//	future.wait();
		//}
	}

	renderTexture->update(pixels.data());
}

/// <summary>
/// Update and draw all ImGui menus.
/// </summary>
void ParticleEffects::handleUI()
{
	if (ImGui::CollapsingHeader("Particles"))
	{
		// Particle properties
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Number of Particles");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::InputInt("Amount", &numOfParticles);		

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Window Coordinates");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		std::string strX = "X: " + std::to_string(startPosition.x);
		std::string strY = "Y: " + std::to_string(startPosition.y);

		ImGui::Text(strX.c_str());
		ImGui::Text(strY.c_str());

		ImGui::Dummy(ImVec2(0.0f, 8.0f));
	}

	// Render output window
	ImGui::Begin("Render##03");

	// Draw content region for debug purposes
	// The content region position is also used to map the
	// mouse position correctly
	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	vMin.x += ImGui::GetWindowPos().x;
	vMin.y += ImGui::GetWindowPos().y;
	vMax.x += ImGui::GetWindowPos().x;
	vMax.y += ImGui::GetWindowPos().y;

	ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));

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

	//main_RT->clear(sf::Color::Transparent);

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
