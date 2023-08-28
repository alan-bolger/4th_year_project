#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include <random>

constexpr auto PI = 3.141592653589793;

class Particle
{
public:
	bool alive;

	Particle() { alive = false;	gen.seed(rd());	};
	~Particle() { };
	virtual void generate(sf::Vector2f startPosition) = 0;
	virtual void update(const sf::Time &dt, sf::RenderTarget &renderTexture) = 0;

protected:
	std::random_device rd;
	std::mt19937 gen;
	sf::Vector2f position;
	sf::Vector2f velocity;
	float speed = 0.0f;
	float angle = 0.0f;
	float modifierTimeStep = 0.0f;
	sf::Vector2f acceleration;
	int modifierValue = 0;
	sf::Time timeToLive;
	sf::RectangleShape particle;
};

#endif // !PARTICLE_H