#include "Raytracer.h"

/// <summary>
/// Raytracer constructor.
/// </summary>
/// <param name="w">The width of the render.</param>
/// <param name="h">The height of the render.</param>
Raytracer::Raytracer(int w, int h) : renderW(w), renderH(h)
{
	renderTexture = std::make_unique<sf::RenderTexture>();
	renderTexture->create(renderW, renderH);
}

/// <summary>
/// Raytracer destructor.
/// </summary>
Raytracer::~Raytracer()
{

}

/// <summary>
/// Update and draw all ImGui menus.
/// </summary>
void Raytracer::handleUI()
{
	if (ImGui::CollapsingHeader("Raytracer"))
	{
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::Text("Select single-threaded\nor multi-threaded\nrendering");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::PushItemWidth(-1);

		static int sel = 0;
		ImGui::RadioButton("Single-threaded", &sel, 0);
		ImGui::RadioButton("Multi-threaded", &sel, 1);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		if (ImGui::Button("RENDER", ImVec2(60, 24)))
		{
			if (sel == 0)
			{
				// Do single threaded
			}
			else if (sel == 1)
			{
				// Do multi threaded
			}
		}

		ImGui::Dummy(ImVec2(0.0f, 8.0f));
	}

	// Render output window
	ImGui::Begin("Render Result");

	ImGui::Image(renderTexture->getTexture());

	ImGui::End();
}
