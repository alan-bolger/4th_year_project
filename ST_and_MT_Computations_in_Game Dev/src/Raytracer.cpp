#include "Raytracer.h"

/// <summary>
/// Raytracer constructor.
/// </summary>
/// <param name="w">The width of the render.</param>
/// <param name="h">The height of the render.</param>
Raytracer::Raytracer(int w, int h) : renderW(w), renderH(h)
{
	renderTexture = std::make_unique<sf::Texture>();
	renderTexture->create(renderW, renderH);

	threadPool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency());

	pixelArray.resize(renderW * renderH * 4);

    // Set startup defaults
    renderTileW = 64;
    renderTileH = 64;
    maxBounces = 5;
    backgroundColour = 1.0f;

    // This sphere acts as the ground
    spheres.push_back(Sphere(Vec3f(0.0, -10000, -5), 10000, Vec3f(0.149, 0.509, 0.192), 1, 0, 0));

    // Some other spheres
    spheres.push_back(Sphere(Vec3f(-3.0, 0.4, -10.0), 0.4, Vec3f(0.835, 0.443, 0.125), 0.1, 1, 0));
    spheres.push_back(Sphere(Vec3f(-2.3, 0.2, -15.0), 0.2, Vec3f(0.713, 0.227, 0.631), 0.2, 1, 0));
    spheres.push_back(Sphere(Vec3f(0.2, 2.3, -24.0), 2.3, Vec3f(0.721, 0.721, 0.721), 1, 1, 0));
    spheres.push_back(Sphere(Vec3f(14.1, 6.0, -53.0), 6.0, Vec3f(0.835, 0.443, 0.125), 0.1, 0.7, 0));
    spheres.push_back(Sphere(Vec3f(-59.1, 24.0, -204.0), 24.0, Vec3f(0.443, 0.835, 0.125), 0.1, 0.7, 0));

    // Render demo image
    render(multiThreaded);
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
    ImGui::PushItemWidth(0);

    if (ImGui::CollapsingHeader("Raytracer"))
    {
        if (ImGui::CollapsingHeader("Output"))
        {
            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::SeparatorText("Render Dimensions");

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::InputInt("Width##01", &renderW);
            ImGui::InputInt("Height##02", &renderH);

            // Limit max to 4k resolution (3840 x 2160)
            renderW = std::clamp(renderW, 128, 3840);
            renderH = std::clamp(renderH, 128, 2160);

            // Render tile options
            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::SeparatorText("Tile Dimensions");
            ImGui::TextWrapped("This only applies to multi-threaded rendering");

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::InputInt("Width##03", &renderTileW);
            ImGui::InputInt("Height##04", &renderTileH);

            // Limit max to 256 x 256
            renderTileW = std::clamp(renderTileW, 16, 256);
            renderTileH = std::clamp(renderTileH, 16, 256);

            ImGui::Dummy(ImVec2(0.0f, 8.0f));
        }

        if (ImGui::CollapsingHeader("Editing"))
        {
            ImGui::SeparatorText("Camera");

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::DragFloat("X", &rayOrigin.x);
            ImGui::DragFloat("Y", &rayOrigin.y);
            ImGui::DragFloat("Z", &rayOrigin.z);

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::ColorEdit3("Background Colour", backgroundColour.get());

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::SeparatorText("Edit Sphere");
            ImGui::TextWrapped("Select the sphere to edit, then click the 'OPEN EDITOR' button to display the sphere editor if it's not already visible");

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            // Display all spheres in the scene in a selectable list
            for (int i = 0; i < spheres.size(); i++)
            {
                std::string text = "Sphere " + std::to_string(i);

                ImGui::Separator();

                if (ImGui::Selectable(text.c_str(), i == activeSphereIndex))
                {
                    // If the user clicks on the sphere, set it as the active sphere
                    activeSphereIndex = i;
                }
            }

            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            if (ImGui::Button("ADD SPHERE", ImVec2(110, 24)))
            {
                spheres.push_back(Sphere(Vec3f(0, 0, 0), 3, Vec3f(0.5, 0.5, 0.5), 0, 0, 0));
            }

            ImGui::SameLine();

            if (ImGui::Button("DELETE SPHERE", ImVec2(110, 24)))
            {
                if (spheres.size() > 0)
                {
                    if (activeSphereIndex < spheres.size())
                    {
                        auto itr = spheres.begin() + activeSphereIndex;
                        spheres.erase(itr);
                    }

                    if (spheres.empty())
                    {
                        activeSphereIndex = -1;
                    }
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("OPEN EDITOR", ImVec2(110, 24)))
            {
                sphereEditWindowOpen = true;
            }

            ImGui::Dummy(ImVec2(0.0f, 8.0f));
        }

        if (ImGui::CollapsingHeader("Render"))
        {
            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::TextWrapped("Increasing max bounces will increase rendering time, but will produce higher quality renders");

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::InputInt("Max Bounces", &maxBounces);

            // Limit to a maximum of 128 potential bounces
            maxBounces = std::clamp(maxBounces, 1, 128);

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::SeparatorText("Thread Usage");

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            static int sel = 1;
            ImGui::RadioButton("Single-threaded", &sel, 0);
            ImGui::RadioButton("Multi-threaded", &sel, 1);
            sel == 0 ? multiThreaded = false : multiThreaded = true;

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            ImGui::TextWrapped("Any changes made will only be visible once the scene has been rendered");

            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            if (ImGui::Button("RENDER", ImVec2(60, 24)))
            {
                render(multiThreaded);
            }

            ImGui::Dummy(ImVec2(0.0f, 8.0f));
        }
    }

    // Render sphere editor window if it's open
    if (sphereEditWindowOpen)
    {
        ImGui::Begin("Edit Sphere", &sphereEditWindowOpen);

        // Options for adding a sphere to the scene
        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        std::string text = "Sphere " + std::to_string(activeSphereIndex);

        ImGui::SeparatorText(text.c_str());

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::DragFloat("Radius", &spheres[activeSphereIndex].radius);
        spheres[activeSphereIndex].radius2 = spheres[activeSphereIndex].radius * spheres[activeSphereIndex].radius;

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::DragFloat("X", &spheres[activeSphereIndex].center.x);
        ImGui::DragFloat("Y", &spheres[activeSphereIndex].center.y);
        ImGui::DragFloat("Z", &spheres[activeSphereIndex].center.z);

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::ColorEdit3("Colour", spheres[activeSphereIndex].surfaceColour.get());

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::DragFloat("Reflection", &spheres[activeSphereIndex].reflection, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Transparency", &spheres[activeSphereIndex].transparency, 0.01f, 0.0f, 1.0f);

        // Clamp values between 0.0 and 1.0
        spheres[activeSphereIndex].reflection = std::clamp(spheres[activeSphereIndex].reflection, 0.0f, 1.0f);
        spheres[activeSphereIndex].transparency = std::clamp(spheres[activeSphereIndex].transparency, 0.0f, 1.0f);

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::ColorEdit3("Emission Colour", spheres[activeSphereIndex].emissionColour.get());

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        if (ImGui::Button("RENDER", ImVec2(60, 24)))
        {
            render(multiThreaded);
        }

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::End();
    }

	// Render output window
	ImGui::Begin("Raytracer");

    renderTexture->update(pixelArray.data());
	ImGui::Image(*renderTexture);

	ImGui::End();
}

/// <summary>
/// This function divides the screen into sections.
/// Each section can then be processed individually on its own thread.
/// </summary>
/// <param name="multiThreaded">False for single-threaded and true for multi-threaded.</param>
void Raytracer::render(bool multiThreaded)
{
    pixelArray.clear();
    pixelArray.resize(renderW * renderH * 4);

    renderTexture = std::make_unique<sf::Texture>();
    renderTexture->create(renderW, renderH);

	if (multiThreaded)
	{
        int numOfSectionsW = (renderW / renderTileW) + 1;
        int numOfSectionsH = (renderH / renderTileH) + 1;

		// Add the render jobs to the thread pool
        for (int y = 0; y < numOfSectionsH; ++y)
        {
            for (int x = 0; x < numOfSectionsW; ++x)
            {
                // This lambda adds a block of code to the thread pool as a job
                threadPool->addJob([=]
                {
                    int startX = x * renderTileW;
                    int startY = y * renderTileH;

                    int endX = startX + renderTileW;
                    int endY = startY + renderTileH;

                    renderSection(sf::Vector2i(startX, startY), sf::Vector2i(endX, endY));
                });
            }
        }
	}
	else
	{
		// This renders using a single thread (this thread, the main thread)
        // The entire image is rendered in one sweep
        renderSection({ 0, 0 }, { renderW, renderH });
	}
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
    float tNear = INFINITY;
    const Sphere *sphere = nullptr;

    // Find the intersection of this ray with the sphere in the scene
    for (unsigned i = 0; i < spheres.size(); ++i)
    {
        float t0 = INFINITY;
        float t1 = INFINITY;

        if (spheres[i].intersect(rayOrigin, rayDir, t0, t1))
        {
            if (t0 < 0)
            {
                t0 = t1;
            }

            if (t0 < tNear)
            {
                tNear = t0;
                sphere = &spheres[i];
            }
        }
    }

    // If there's no intersection then return black or background color
    if (!sphere)
    {
        return backgroundColour;
    }

    Vec3f surfaceColour = 0; // The colour of the surface at the ray intersection point
    Vec3f pHit = rayOrigin + rayDir * tNear; // The point of intersection
    Vec3f nHit = pHit - sphere->center; // The normal at the intersection point

    nHit.normalize();

    // If the normal and the view direction are not opposite to each other then
    // reverse the normal direction. That also means we are inside the sphere, so set
    // the inside bool to true. Finally, reverse the sign of IdotN which we want
    // positive
    float bias = 1e-4; // Add some bias to the point from which we will be tracing
    bool inside = false;

    if (rayDir.dot(nHit) > 0)
    {
        nHit = -nHit;
        inside = true;
    }

    if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < maxBounces)
    {
        float facingRatio = -rayDir.dot(nHit);

        // Change the mix value to tweak the effect
        float fresnelEffect = mix(pow(1 - facingRatio, 3), 1, 0.1);

        // Compute reflection direction (no need to normalize because all vectors
        // are already normalized)
        Vec3f reflDir = rayDir - nHit * 2 * rayDir.dot(nHit);
        reflDir.normalize();

        Vec3f reflection = trace(pHit + nHit * bias, reflDir, depth + 1);
        Vec3f refraction = 0;

        // If the sphere is also transparent then compute refraction ray (transmission)
        if (sphere->transparency)
        {
            float ior = 1.1;
            float eta = (inside) ? ior : 1 / ior; // Are we inside or outside the surface?
            float cosI = -nHit.dot(rayDir);
            float k = 1 - eta * eta * (1 - cosI * cosI);

            Vec3f refrDir = rayDir * eta + nHit * (eta * cosI - std::sqrt(k));
            refrDir.normalize();

            refraction = trace(pHit - nHit * bias, refrDir, depth + 1);
        }

        // The result is a mix of reflection and refraction (if the sphere is transparent)
        surfaceColour = (reflection * fresnelEffect + refraction * (1 - fresnelEffect) * sphere->transparency) * sphere->surfaceColour;
    }
    else
    {
        // It's a diffuse object so there's no need to trace any more
        for (unsigned i = 0; i < spheres.size(); ++i)
        {
            if (spheres[i].emissionColour.x > 0)
            {
                // This is a light
                Vec3f transmission = 1;
                Vec3f lightDirection = spheres[i].center - pHit;
                lightDirection.normalize();

                for (unsigned j = 0; j < spheres.size(); ++j)
                {
                    if (i != j)
                    {
                        float t0 = 0.0f;
                        float t1 = 0.0f;

                        if (spheres[j].intersect(pHit + nHit * bias, lightDirection, t0, t1))
                        {
                            transmission = 0;
                            break;
                        }
                    }
                }

                surfaceColour += sphere->surfaceColour * transmission * std::max(0.0f, nHit.dot(lightDirection)) * spheres[i].emissionColour;
            }
        }
    }

    return surfaceColour + sphere->emissionColour;
}

/// <summary>
/// Raytrace a section of the image.
/// We compute a camera ray for each pixel of the image,
/// trace it and return a colour. If the ray hits a sphere, we return the colour of the
/// sphere at the intersection point, otherwise we return the background colour.
/// </summary>
/// <param name="pixTL">The coordinate of the upper-left corner of the section.</param>
/// <param name="pixBR">The coordinate of the lower-right corner of the section.</param>
void Raytracer::renderSection(sf::Vector2i pixTL, sf::Vector2i pixBR)
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

            // Out of bounds check
            if (index < 0 || (index * 4) > (pixelArray.size() - 1))
            {
                continue;
            }

			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectRatio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;

			Vec3f rayDir(xx, yy, -1);
			rayDir.normalize();
			Vec3f pixel = trace(rayOrigin, rayDir, 0);

			// Update pixel array - RGBA
			pixelArray[index * 4] = (uint8_t)(std::min(1.0f, pixel.x) * 255);
			pixelArray[(index * 4) + 1] = (uint8_t)(std::min(1.0f, pixel.y) * 255);
			pixelArray[(index * 4) + 2] = (uint8_t)(std::min(1.0f, pixel.z) * 255);
			pixelArray[(index * 4) + 3] = 255;
		}
	}
}