#include "DefaultParticle.h"

/// <summary>
/// DefaultParticle constructor.
/// </summary>
DefaultParticle::DefaultParticle()
{
	particle.setSize(sf::Vector2f(2, 2));
	particle.setOrigin(particle.getSize() / 2.0f);
	particle.setFillColor(sf::Color(255, 255, 0, 255));

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
/// <param name="startPosition">The starting position of the particle.</param>
void DefaultParticle::generate(sf::Vector2f startPosition)
{
	// Set initial properties;
	position = startPosition;

	// Random angle between 0 and 359
	std::uniform_real_distribution<double> angleDist(0.0, 359.0);
	angle = angleDist(gen) * (PI / 180.0);

	// Random time between 4.0 and 8.0 seconds
	std::uniform_real_distribution<double> timeToLiveDist(4.0, 20.0);
	timeToLive = sf::seconds(timeToLiveDist(gen));

	// Set modifier step value (the amount the modifier changes on each frame)
	// For this particle, the value starts at 255 and gets decreased on every frame
	// The value should hit 0 just as the time to live timer runs out
	// It is used to change the green and alpha values so the particle changes
	// from yellow to red and fades out
	modifierTimeStep = 255 / (timeToLive.asSeconds() / (1.0f / 60.0f));
	modifierValue = 255;

	// Random speed between 20.0 and 40.0 units
	std::uniform_real_distribution<double> f_speedDist(20.0, 40.0);
	speed = f_speedDist(gen);

	// Set initial velocity vector
	velocity.x = speed * std::cos(angle);
	velocity.y = speed * std::sin(angle);

	alive = true; // It's alive!
}

/// <summary>
/// Update.
/// </summary>
/// <param name="dt">Delta time.</param>
/// <param name="renderTexture">A reference to a render texture.</param>
void DefaultParticle::update(const sf::Time &dt, sf::RenderTarget &renderTexture)
{
	timeToLive -= dt;

	velocity.x = speed * std::cos(angle);
	velocity.y = speed * std::sin(angle);

	position.x += velocity.x * dt.asSeconds();
	position.y += velocity.y * dt.asSeconds();

	particle.setFillColor(sf::Color(255, modifierValue, 0, modifierValue));
	particle.setPosition(position);

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

	renderTexture.draw(particle);
}
