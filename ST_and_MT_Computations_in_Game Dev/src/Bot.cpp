#include "Bot.h"

/// <summary>
/// Bot constructor.
/// </summary>
/// <param name="x">The bot's X position (tile coordinates).</param>
/// <param name="y">The bot's Y position (tile coordinates).</param>
/// <param name="mapData">The map to perform pathfinding on.</param>
Bot::Bot(int x, int y, const std::vector<int> &mapData) : position(x, y)
{
	aStar = std::make_unique<AStar>(mapData, 256, 256);
	encapsulatedAStar = true;

	texture.loadFromFile("assets/dungeon_characters.png");

	sprite.setTexture(texture);
	sprite.setTextureRect({ 0, 32, 16, 16 });
	sprite.setPosition(x * 16, y * 16);
}

/// <summary>
/// Bot destructor.
/// </summary>
Bot::~Bot()
{

}

/// <summary>
/// Start pathfinding.
/// </summary>
/// <param name="x">The destination tile's X position (tile coordinates).</param>
/// <param name="y">The destination tile's Y position (tile coordinates).</param>
void Bot::startPathfinding(int x, int y)
{
	// Perform pathfinding
	aStar->run(position, { x, y });

	// Reset clock
	clock.restart();
	timer = sf::Time::Zero;
}

/// <summary>
/// Gets the bots current position (in tile coordinates).
/// </summary>
/// <returns>The tile coordinate that the bot occupies.</returns>
sf::Vector2i Bot::getPosition()
{
	return position;
}

/// <summary>
/// Update.
/// </summary>
/// <param name="botSpeed">The time interval between bot movements in seconds.</param>
void Bot::update(float botSpeed)
{
	timer += clock.restart();

	if (timer > sf::seconds(botSpeed))
	{
		// Only move bot while the list contains coordinates
		if (!aStar->getPath()->empty())
		{
			// Get the next node
			sf::Vector2i nextNode = aStar->getPath()->front();

			// Move position
			position = nextNode;

			// Pop current node
			aStar->getPath()->pop_front();

			// Set sprite's new position
			sprite.setPosition(position.x * 16, position.y * 16);
		}

		timer = sf::Time::Zero;
	}
}

/// <summary>
/// Draw bots.
/// </summary>
/// <param name="target">A render target.</param>
/// <param name="drawPath">Set to true to draw path.</param>
void Bot::draw(sf::RenderTarget &target, bool drawPath)
{
	if (drawPath)
	{
		// Draw the path
		sf::RectangleShape rect({ 16, 16 });
		rect.setOutlineThickness(-1.0f);
		rect.setFillColor(sf::Color::Transparent);

		for (auto &node : *aStar->getPath())
		{
			rect.setPosition(node.x * 16, node.y * 16);
			target.draw(rect);
		}
	}

	// Draw the sprite
	target.draw(sprite);
}
