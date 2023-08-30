// --------------------------------------------
// Particle.h
// --------------------------------------------
// This is the particle base class.
// --------------------------------------------

#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include <random>

class Particle
{
public:
	bool alive;

	Particle(std::mt19937 &gen) : gen(gen)
	{
		alive = false; 
	};

	~Particle() { };
	void setSpeed(float speed) { this->speed = speed; };
	void setLifetime(sf::Time timeToLive) { this->timeToLive = timeToLive; };
	virtual void generate(sf::Vector2f startPosition, float speed, float timeToLive) = 0;
	virtual void update(const sf::Time &dt, std::vector<uint8_t> &pixels, int scrW) = 0;

protected:
	const double PI = 3.141592653589793;
	std::mt19937 &gen;
	sf::Vector2f position;
	sf::Vector2f velocity;
	float speed = 5.0f;
	float angle = 0.0f;
	float modifierTimeStep = 0.0f;
	sf::Vector2f acceleration;
	int modifierValue = 0;
	sf::Time timeToLive;
};

#endif // !PARTICLE_H