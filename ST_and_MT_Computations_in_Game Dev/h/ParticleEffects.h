// --------------------------------------------
// ParticleEffects.h
// ParticleEffects.cpp
// --------------------------------------------
// Test particle effects on multiple threads.
// --------------------------------------------

#ifndef PARTICLEEFFECTS_H
#define PARTICLEEFFECTS_H

#include <SFML/Graphics.hpp>

class ParticleEffects
{
public:
	ParticleEffects();
	~ParticleEffects();
	void update(const sf::Time &dt);
	void handleUI();
	void render();

private:

};

#endif // !PARTICLEEFFECTS_H