#include "DefaultParticle.h"

/// <summary>
/// DefaultParticle constructor.
/// </summary>
DefaultParticle::DefaultParticle()
{
	acceleration = sf::Vector2f(0.0f, 0.0f);
}

/// <summary>
/// DefaultParticle destructor.
/// </summary>
DefaultParticle::~DefaultParticle()
{

}
/// <summary>
/// Generate a particle.
/// </summary>

/// <summary>
/// Generate a particle.
/// </summary>
/// <param name="startPosition">The starting position of the particle.</param>
/// <param name="speed">The max speed of the particle.</param>
/// <param name="timeToLive">The max time for the particle to live.</param>
void DefaultParticle::generate(sf::Vector2f startPosition, float speed, float timeToLive)
{
	// Set initial properties;
	position = startPosition;

	// Random angle between 0 and 359
	std::uniform_real_distribution<double> angleDist(0.0, 360.0);
	angle = angleDist(gen) * (PI / 180.0);

	// Random time between 4.0 and timeToLive
	std::uniform_real_distribution<double> timeToLiveDist(4.0, timeToLive);
	this->timeToLive = sf::seconds(timeToLiveDist(gen));

	// Set modifier step value (the amount the modifier changes on each frame)
	// For this particle, the value starts at 255 and gets decreased on every frame
	// The value should hit 0 just as the time to live timer runs out
	// It is used to change the green and alpha values so the particle changes
	// from yellow to red and fades out
	modifierTimeStep = 255 / (this->timeToLive.asSeconds() / (1.0f / 60.0f));
	modifierValue = 255;

	// Random speed between 20.0 and 40.0 units
	std::uniform_real_distribution<double> f_speedDist(0.5, speed);
	this->speed = f_speedDist(gen);

	// Set initial velocity vector
	velocity.x = this->speed * std::cos(angle);
	velocity.y = this->speed * std::sin(angle);

	alive = true; // It's alive!
}

/// <summary>
/// Update.
/// </summary>
/// <param name="dt">Delta time.</param>
/// <param name="pixels">An array containing pixel data.</param>
/// <param name="scrW">The screen/pixel array width.</param>
void DefaultParticle::update(const sf::Time &dt, std::vector<uint8_t> &pixels, int scrW)
{
	timeToLive -= dt;

	velocity.x = speed * std::cos(angle);
	velocity.y = speed * std::sin(angle);

	position.x += velocity.x * dt.asSeconds();
	position.y += velocity.y * dt.asSeconds();

	if (timeToLive <= sf::seconds(0.01f))
	{
		alive = false;
		return;
	}

	modifierValue -= modifierTimeStep;

	if (modifierValue < 0)
	{
		modifierValue = 0;
	}

	int index = (static_cast<int>(position.y) * scrW + static_cast<int>(position.x)) * 4;

	if (index >= 0 && index < pixels.size())
	{
		pixels[index] = 255;
		pixels[index + 1] = modifierValue;
		pixels[index + 2] = 0;
		pixels[index + 3] = modifierValue;
	}
}
