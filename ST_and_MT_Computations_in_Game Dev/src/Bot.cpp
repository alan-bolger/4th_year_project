#include "Bot.h"

/// <summary>
/// Bot constructor.
/// </summary>
/// <param name="x">The bot's X position (tile coordinates).</param>
/// <param name="y">The bot's Y position (tile coordinates).</param>
Bot::Bot(int x, int y)
{
	texture.loadFromFile("assets/dungeon_characters.png");

	sprite.setTexture(texture);
	sprite.setTextureRect({ 0, 0, 16, 16 });
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
void Bot::start(int x, int y)
{
	// Perform pathfinding
	// Create list of path nodes
	// Switch to movement mode
	// Follow list of path nodes to destination
	// Profit
}

/// <summary>
/// Update.
/// </summary>
/// <param name="dt">Delta time.</param>
void Bot::update(const sf::Time &dt)
{
	// Only move while the list contains something
}

/// <summary>
/// Draw bots.
/// </summary>
/// <param name="target"></param>
void Bot::draw(sf::RenderTarget &target)
{
	target.draw(sprite);
}
