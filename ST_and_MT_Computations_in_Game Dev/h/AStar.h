// --------------------------------------------
// AStar.h
// AStar.cpp
// --------------------------------------------
// This class provides A* pathfinding.
// --------------------------------------------

#ifndef ASTAR_H
#define ASTAR_H

#include <SFML/Graphics.hpp>
#include <list>

class AStar
{
public:
	AStar(const std::vector<int> &mapData);
	~AStar();
	std::list<sf::Vector2f> run(sf::Vector2f start, sf::Vector2f end);

private:
	struct Node
	{
		int x;
		int y;
		float globalGoal;
		float localGoal;
		bool obstacle = false;
		bool visited = false;
		std::vector<Node*> neighbours;
		Node *parent;
	};

	Node *nodes = nullptr;
	Node *nodeStart = nullptr;
	Node *nodeEnd = nullptr;
	int mapWidth = 16;
	int mapHeight = 16;
	std::list<sf::Vector2f> path;

	void initialise(const std::vector<int> &mapData);
};

#endif // !ASTAR_H