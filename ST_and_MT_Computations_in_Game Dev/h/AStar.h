// --------------------------------------------
// AStar.h
// AStar.cpp
// --------------------------------------------
// This class provides A* pathfinding.
// Contains code taken and modified from
// JavidX9
// --------------------------------------------

#ifndef ASTAR_H
#define ASTAR_H

#include <SFML/Graphics.hpp>
#include <list>

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

class AStar
{
public:
	AStar(const std::vector<int> &mapData, int mapWidth, int mapHeight);
	~AStar();
	Node *getNodes();
	Node *getNodeStart();
	Node *getNodeEnd();
	void run(sf::Vector2i start, sf::Vector2i end);
	std::list<sf::Vector2i> *getPath();

private:
	Node *nodes = nullptr;
	Node *nodeStart = nullptr;
	Node *nodeEnd = nullptr;
	int mapWidth;
	int mapHeight;
	std::list<sf::Vector2i> path;

	void initialise(const std::vector<int> &mapData);
};

#endif // !ASTAR_H