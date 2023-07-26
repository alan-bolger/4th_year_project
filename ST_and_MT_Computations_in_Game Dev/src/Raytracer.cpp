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

/// <summary>
/// This function divides the screen into sections.
/// Each section can then be processed individually on its own thread.
/// </summary>
void Raytracer::render()
{
	// This renders using multi-threading
	int numOfSections = 16;
	int sectionWidth = renderW / numOfSections;

	// Add the render jobs to the thread pool
	for (int i = 0; i < numOfSections; ++i)
	{
		threadPool->addJob([=]
			{
				int startX = i * sectionWidth;
				int endX = startX + sectionWidth;
				renderSection(sf::Vector2f(startX, 0), sf::Vector2f(endX, renderH));
			});
	}

	// This renders using a single thread
	// renderSection({ 0, 0 }, { 1280, 720 });
}

/// <summary>
/// Mix utility function.
/// </summary>
/// <param name="a">First value to mix.</param>
/// <param name="b">Second value to mix.</param>
/// <param name="mix">Mix value.</param>
/// <returns>The resulting mixed value.</returns>
float Raytracer::mix(const float &a, const float &b, const float &mix)
{
	return b * mix + a * (1 - mix);
}

/// <summary>
/// This is the main trace function. It takes a ray as argument (defined by its origin
/// and direction). We test if this ray intersects any of the geometry in the scene.
/// If the ray intersects an object, we compute the intersection point, the normal
/// at the intersection point, and shade this point using this information.
/// Shading depends on the surface property (is it transparent, reflective or diffuse?).
/// The function returns a colour for the ray. If no intersection occurs, the background
/// colour is returned.
/// </summary>
/// <param name="rayOrigin">The origin point of the ray.</param>
/// <param name="rayDir">The direction of the ray.</param>
/// <param name="depth">Depth of recursion.</param>
/// <returns>The ray colour.</returns>
Vec3f Raytracer::trace(const Vec3f &rayOrigin, const Vec3f &rayDir, const int &depth)
{
	return Vec3f();
}

/// <summary>
/// Raytrace a section of the image.
/// We compute a camera ray for each pixel of the image,
/// trace it and return a colour. If the ray hits a sphere, we return the colour of the
/// sphere at the intersection point, otherwise we return the background colour.
/// </summary>
/// <param name="pixTL">The coordinate of the upper-left corner of the section.</param>
/// <param name="pixBR">The coordinate of the lower-right corner of the section.</param>
void Raytracer::renderSection(sf::Vector2f pixTL, sf::Vector2f pixBR)
{
	float invWidth = 1.0f / static_cast<float>(renderW);
	float invHeight = 1.0f / static_cast<float>(renderH);
	float aspectRatio = static_cast<float>(renderW) / static_cast<float>(renderH);
	float angle = std::tan(PI * 0.5f * fov / 180.0f);

	for (unsigned int y = pixTL.y; y < pixBR.y; ++y)
	{
		for (unsigned int x = pixTL.x; x < pixBR.x; ++x)
		{
			int index = (y * renderW + x);

			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectRatio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;

			Vec3f rayDir(xx, yy, -1);
			rayDir.normalize();
			Vec3f pixel = trace(rayOrigin, rayDir, 0);

			pixelArray[index * 4] = (uint8_t)(std::min(1.0f, pixel.x) * 255);
			pixelArray[(index * 4) + 1] = (uint8_t)(std::min(1.0f, pixel.y) * 255);
			pixelArray[(index * 4) + 2] = (uint8_t)(std::min(1.0f, pixel.z) * 255);
			pixelArray[(index * 4) + 3] = 255;
		}
	}
}