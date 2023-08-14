#include "AStar.h"

/// <summary>
/// AStar constructor.
/// </summary>
/// <param name="data">An array of integers representing the map.</param>
AStar::AStar(const std::vector<int> &mapData)
{
	initialise(mapData);
}

/// <summary>
/// AStar destructor.
/// </summary>
AStar::~AStar()
{

}

/// <summary>
/// This function runs the A* algorithm.
/// </summary>
/// <param name="start">The starting node.</param>
/// <param name="end">The destination node.</param>
/// <returns>A list containing the best possible path.</returns>
std::list<sf::Vector2f> AStar::run(sf::Vector2f start, sf::Vector2f end)
{
	// Set start and end node
	nodeStart = &nodes[(int)start.y * mapWidth + (int)start.x];
	nodeEnd = &nodes[(int)end.y * mapWidth + (int)end.x];

	// Reset the navigation graph
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			nodes[y * mapWidth + x].visited = false;
			nodes[y * mapWidth + x].globalGoal = INFINITY;
			nodes[y * mapWidth + x].localGoal = INFINITY;
			nodes[y * mapWidth + x].parent = nullptr;
		}
	}

	// Distance lambda function
	auto distance = [](Node *a, Node *b)
	{
		return std::sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
	};

	// Heuristic lambda function
	auto heuristic = [distance](Node *a, Node *b) // So we can experiment with heuristic
	{
		return distance(a, b);
	};

	// Setup starting conditions
	Node *nodeCurrent = nodeStart;
	nodeStart->localGoal = 0.0f;
	nodeStart->globalGoal = heuristic(nodeStart, nodeEnd);

	// Add start node to not tested list - this will ensure it gets tested
	// As the algorithm progresses, newly discovered nodes get added to this
	// list and will themselves be tested later
	std::list<Node*> listNotTestedNodes;
	listNotTestedNodes.push_back(nodeStart);

	// If the not tested list contains nodes, there may be better paths
	// which have not yet been explored. However, we will also stop 
	// searching when we reach the target - there may well be better
	// paths, but this one will do - it won't be the longest
	while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd)
	{
		// Sort untested nodes by global goal so that the lowest is first
		listNotTestedNodes.sort([](const Node *lhs, const Node *rhs) { return lhs->globalGoal < rhs->globalGoal; });

		// Front of listNotTestedNodes is potentially the lowest distance node. Our
		// list may also contain nodes that have been visited, so ditch these...
		while (!listNotTestedNodes.empty() && listNotTestedNodes.front()->visited)
		{
			listNotTestedNodes.pop_front();
		}			

		// Abort because there are no valid nodes left to test
		if (listNotTestedNodes.empty())
		{
			break;
		}			

		nodeCurrent = listNotTestedNodes.front();
		nodeCurrent->visited = true; // We only explore a node once

		// Check each of this node's neighbours
		for (auto nodeNeighbour : nodeCurrent->neighbours)
		{
			// If the neighbour is not visited and is 
			// not an obstacle, add it to not tested list
			if (!nodeNeighbour->visited && nodeNeighbour->obstacle == 0)
			{
				listNotTestedNodes.push_back(nodeNeighbour);
			}				

			// Calculate the neighbour's potential lowest parent distance
			float possiblyLowerGoal = nodeCurrent->localGoal + distance(nodeCurrent, nodeNeighbour);

			// If choosing the path through this node is a lower distance than what 
			// the neighbour currently has set, update the neighbour to use this node
			// as the path source and set its distance scores as necessary
			if (possiblyLowerGoal < nodeNeighbour->localGoal)
			{
				nodeNeighbour->parent = nodeCurrent;
				nodeNeighbour->localGoal = possiblyLowerGoal;

				// The best path length to the neighbour being tested has changed, so
				// update the neighbour's score. The heuristic is used to globally bias
				// the path algorithm so that it knows if it's getting better or worse. At some
				// point the algorithm will realise this path is worse and abandon it, and then go
				// and search along the next best path
				nodeNeighbour->globalGoal = nodeNeighbour->localGoal + heuristic(nodeNeighbour, nodeEnd);
			}
		}
	}

	return path;
}

/// <summary>
/// Initialise A*
/// </summary>
/// <param name="mapData">An array of integers representing the map.</param>
void AStar::initialise(const std::vector<int> &mapData)
{
	// Create nodes
	nodes = new Node[mapWidth * mapHeight];

	for (int x = 0; x < mapWidth; ++x)
	{
		for (int y = 0; y < mapHeight; ++y)
		{
			nodes[y * mapWidth + x].x = x;
			nodes[y * mapWidth + x].y = y;
			nodes[y * mapWidth + x].obstacle = false;
			nodes[y * mapWidth + x].parent = nullptr;
			nodes[y * mapWidth + x].visited = false;

			if (mapData[y * mapWidth + x] != 0)
			{
				nodes[y * mapWidth + x].obstacle = true;
			}
		}
	}

	// Create connections
	for (int x = 0; x < mapWidth; ++x)
	{
		for (int y = 0; y < mapHeight; ++y)
		{
			// These are straight horizontal and vertical connections
			if (y > 0)
			{
				nodes[y * mapWidth + x].neighbours.push_back(&nodes[(y - 1) * mapWidth + (x + 0)]);
			}

			if (y < mapHeight - 1)
			{
				nodes[y * mapWidth + x].neighbours.push_back(&nodes[(y + 1) * mapWidth + (x + 0)]);
			}

			if (x > 0)
			{
				nodes[y * mapWidth + x].neighbours.push_back(&nodes[(y + 0) * mapWidth + (x - 1)]);
			}

			if (x < mapWidth - 1)
			{
				nodes[y * mapWidth + x].neighbours.push_back(&nodes[(y + 0) * mapWidth + (x + 1)]);
			}

			// These are diagonal connections
			if (y > 0 && x > 0)
			{
				nodes[y * mapWidth + x].neighbours.push_back(&nodes[(y - 1) * mapWidth + (x - 1)]);
			}

			if (y < mapHeight - 1 && x > 0)
			{
				nodes[y * mapWidth + x].neighbours.push_back(&nodes[(y + 1) * mapWidth + (x - 1)]);
			}

			if (y > 0 && x < mapWidth - 1)
			{
				nodes[y * mapWidth + x].neighbours.push_back(&nodes[(y - 1) * mapWidth + (x + 1)]);
			}

			if (y < mapHeight - 1 && x < mapWidth - 1)
			{
				nodes[y * mapWidth + x].neighbours.push_back(&nodes[(y + 1) * mapWidth + (x + 1)]);
			}
		}
	}

	// Place start and end nodes to avoid null pointers
	nodeStart = &nodes[2 * mapWidth + 2];
	nodeEnd = &nodes[3 * mapWidth + 3];
}
