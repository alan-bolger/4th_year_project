// --------------------------------------------
// Pathfinding.h
// Pathfinding.cpp
// --------------------------------------------
// This is the pathfinding test. The user can
// place bots on the modifiable map and select
// a target destination that the bots will
// travel to.
// --------------------------------------------

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"
#include "Vec3.h"
#include "Timer.h"

class Pathfinding
{
public:
	Pathfinding();
	~Pathfinding();
	void handleUI();

private:
	int mapWidth = 32;
	int mapHeight = 32;
	int nodeWidth = 8;
	int nodeHeight = 8;
};

#endif // !PATHFINDING_H