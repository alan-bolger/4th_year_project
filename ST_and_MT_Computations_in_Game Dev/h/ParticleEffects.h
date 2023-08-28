// --------------------------------------------
// ParticleEffects.h
// ParticleEffects.cpp
// --------------------------------------------
// Test particle effects on multiple threads.
// --------------------------------------------

#ifndef PARTICLEEFFECTS_H
#define PARTICLEEFFECTS_H

#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"

class ParticleEffects
{
public:
	ParticleEffects();
	~ParticleEffects();
	void update(const sf::Time &dt);
	void handleUI();
	void render();

private:
	std::unique_ptr<sf::RenderTexture> main_RT;
	std::unique_ptr<ThreadPool> threadPool;
	int numOfParticles = 1000;
};

#endif // !PARTICLEEFFECTS_H