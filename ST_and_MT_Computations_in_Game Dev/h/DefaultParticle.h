#ifndef DEFAULTPARTICLE_H
#define DEFAULTPARTICLE_H

#include "Particle.h"

class DefaultParticle : public Particle
{
public:
	DefaultParticle();
	~DefaultParticle();
	void generate(sf::Vector2f startPosition);
	void update(const sf::Time &dt, sf::RenderTarget &renderTexture);
};

#endif // !DEFAULTPARTICLE_H