// --------------------------------------------
// DefaultParticle.h
// DefaultParticle.cpp
// --------------------------------------------
// This is a standard type of particle that
// eminates in a radial formation from the
// current mouse position.
// --------------------------------------------

#ifndef DEFAULTPARTICLE_H
#define DEFAULTPARTICLE_H

#include "Particle.h"

class DefaultParticle : public Particle
{
public:
	DefaultParticle();
	~DefaultParticle();
	void generate(sf::Vector2f startPosition, float speed, float timeToLive);
	void update(const sf::Time &dt, std::vector<uint8_t> &pixels, int scrW);
};

#endif // !DEFAULTPARTICLE_H