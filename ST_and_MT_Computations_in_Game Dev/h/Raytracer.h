// --------------------------------------------
// Raytracer.h
// Raytracer.cpp
// --------------------------------------------
// Raytracer multi threaded test.
// --------------------------------------------

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"
#include "Vec3.h"
#include "Timer.h"

#include <SFML/Graphics.hpp>

struct Sphere
{
    Vec3f center;
    float radius;
    float radius2;
    Vec3f surfaceColour;
    float transparency;
    float reflection;
    Vec3f emissionColour;
    Vec3f rayOrigin;

    /// <summary>
    /// Sphere constructor.
    /// </summary>
    /// <param name="center">The sphere's center point.</param>
    /// <param name="radius">The sphere's radius.</param>
    /// <param name="surfaceColour">The sphere's surface colour.</param>
    /// <param name="reflection">The sphere's reflection value.</param>
    /// <param name="transparency">The sphere's transparency value.</param>
    /// <param name="emission">The sphere's emission colour.</param>
    Sphere(const Vec3f &center, const float &radius, const Vec3f &surfaceColour, const float &reflection = 0.0f, const float &transparency = 0.0f, const Vec3f &emissionColour = 0.0f)
        :
        center(center), radius(radius), radius2(radius *radius), surfaceColour(surfaceColour), reflection(reflection), transparency(transparency), emissionColour(emissionColour) { }

    /// <summary>
    /// Compute a ray to sphere intersection using the geometric solution.
    /// </summary>
    /// <param name="rayOrigin">The origin point of the ray.</param>
    /// <param name="rayDir">The direction of the ray.</param>
    /// <param name="t0">Near value.</param>
    /// <param name="t1">Far value.</param>
    /// <returns>True if there's an intersection, false if there's no intersection.</returns>
    bool intersect(const Vec3f &rayOrigin, const Vec3f &rayDir, float &t0, float &t1) const
    {
        Vec3f l = center - rayOrigin;
        float tca = l.dot(rayDir);

        if (tca < 0) { return false; }

        float d2 = l.dot(l) - tca * tca;

        if (d2 > radius2) { return false; }

        float thc = std::sqrt(radius2 - d2);
        t0 = tca - thc;
        t1 = tca + thc;

        return true;
    }
};

class Raytracer
{
public:
	Raytracer(int w, int h);
	~Raytracer();
    void handleUI();

private:	
    const double PI = 3.141592653589793;
	int renderW;
	int renderH;
    Vec3f rayOrigin{ 0.0f, 0.0f, 10.0f };
    int fov = 30;
    std::unique_ptr<ThreadPool> threadPool;
    std::unique_ptr<sf::Texture> renderTexture;
    std::vector<uint8_t> pixelArray;
    std::vector<Sphere> spheres;
    int renderTileW;
    int renderTileH;
    int maxBounces;
    Vec3f backgroundColour;
    bool multiThreaded = true;
    int activeSphereIndex;
    bool sphereEditWindowOpen = false;

    void render(bool multiThreaded);
    float mix(const float &a, const float &b, const float &mix);
    Vec3f trace(const Vec3f &rayOrigin, const Vec3f &rayDir, const int &depth);
    void renderSection(sf::Vector2i pixTL, sf::Vector2i pixBR);
};

#endif // !RAYTRACER_H