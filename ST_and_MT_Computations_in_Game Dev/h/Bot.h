// --------------------------------------------
// Bot.h
// --------------------------------------------
// An entity that can navigate through a map
// using pathfinding.
// --------------------------------------------

#ifndef BOT_H
#define BOT_H

#include <SFML/Graphics.hpp>

#include "AStar.h"

class Bot
{
public:
    Bot(int x, int y);
    ~Bot();
    void start(int x, int y);
    void update(const sf::Time &dt);
    void draw(sf::RenderTarget &target);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2i position;
};

#endif // !BOT_H