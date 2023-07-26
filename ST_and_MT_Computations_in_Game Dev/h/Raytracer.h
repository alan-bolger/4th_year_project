// --------------------------------------------
// | Raytracer.h							  |
// | Raytracer.cpp							  |
// | -----------------------------------------|
// | This is a fairly simple raytracer that	  |
// | can render spheres.	                  |
// | It can operate on a single thread		  |
// | or on multiple threads.				  |
// --------------------------------------------

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"
#include "Vec3.h"

#include <SFML/Graphics.hpp>

using Vec3f = Vec3<float>;

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

    Sphere(const Vec3f &center, const float &radius, const Vec3f &surfaceColour, const float &reflection = 0.0f, const float &transparency = 0.0f, const Vec3f &emissionColour = 0.0f)
        :
        center(center), radius(radius), radius2(radius *radius), surfaceColour(surfaceColour), reflection(reflection), transparency(transparency), emissionColour(emissionColour) { }

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
	int renderW;
	int renderH;
    std::unique_ptr<sf::RenderTexture> renderTexture;
};

#endif // !RAYTRACER_H