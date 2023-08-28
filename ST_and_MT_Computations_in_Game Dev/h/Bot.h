// --------------------------------------------
// Bot.h
// Bot.cpp
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
    Bot(int x, int y, const std::vector<int> &mapData);
    ~Bot();
    void startPathfinding(int x, int y);
    sf::Vector2i getPosition();
    void update(float botSpeed = 0.5f);
    void draw(sf::RenderTarget &target, bool drawPath);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2i position;
    std::unique_ptr<AStar> aStar;
    AStar *externalAStar;
    sf::Clock clock;
    sf::Time timer;
    bool encapsulatedAStar;
};

#endif // !BOT_H