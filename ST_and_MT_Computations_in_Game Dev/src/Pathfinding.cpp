#include "Pathfinding.h"

/// <summary>
/// Pathfinding constructor.
/// </summary>
Pathfinding::Pathfinding()
{
 	// Configure tile map
	tileSet = std::make_unique<sf::Texture>();
	tileSet->loadFromFile("assets/dungeon_tileset.png");

	layer_0 = std::make_unique<TileMap>();
	layer_1 = std::make_unique<TileMap>();

	layer_0->loadTileMap("assets/pathfinding_map_layer_0.txt", tileSet.get(), tileWidth, tileHeight, mapWidth, mapHeight);
	layer_1->loadTileMap("assets/pathfinding_map_layer_1.txt", tileSet.get(), tileWidth, tileHeight, mapWidth, mapHeight);

	aStar = std::make_unique<AStar>(*layer_1->getTileArray(), mapWidth, mapHeight);

	windowView.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	//windowView.zoom(0.5f);

	tileMap_RT.create(tileWidth * mapWidth, tileHeight * mapHeight);
	debugMap_RT.create(tileWidth * mapWidth, tileHeight * mapHeight);

	// Quick path test
	aStar->run({ 3, 11 }, { 18, 21 });
}

/// <summary>
/// Pathfinding constructor.
/// </summary>
Pathfinding::~Pathfinding()
{

}

/// <summary>
/// Update and draw all ImGui menus.
/// </summary>
void Pathfinding::handleUI()
{
    ImGui::PushItemWidth(0);

    if (ImGui::CollapsingHeader("Map"))
    {
        // Cosmetic options
        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::SeparatorText("Colour Scheme");

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::ColorEdit3("Nodes", nodeColour.get());

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::ColorEdit3("Connections", connectionColour.get());

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::ColorEdit3("Paths", pathColour.get());

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::ColorEdit3("Bots", botColour.get());

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::Checkbox("Show Tile Map", &showTileMap);
		ImGui::Checkbox("Show Debug Map", &showDebugMap);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));
    }

	// Render output window
	ImGui::Begin("Render");

	ImGui::Image(tileMap_RT);
	ImGui::Image(debugMap_RT);

	ImGui::End();

	// Controls for moving map
	float jumpRange = 5.0f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		windowView.move({ 0.0f, -jumpRange });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		windowView.move({ 0.0f, jumpRange });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		windowView.move({ -jumpRange, 0.0f });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		windowView.move({ jumpRange, 0.0f });
	}
}

/// <summary>
/// Draws the map and shows connections, bots, paths, etc.
/// </summary>
void Pathfinding::render()
{
	// Clear render textures
	tileMap_RT.clear(sf::Color(0, 0, 0, 0));
	debugMap_RT.clear(sf::Color(0, 0, 0, 0));

	tileMap_RT.setView(windowView);
	debugMap_RT.setView(windowView);

	if (showTileMap)
	{
		// Draw map
		layer_0->draw(tileMap_RT);
		layer_1->draw(tileMap_RT);
	}

	if (showDebugMap)
	{
		// Draw nodes and connections
		sf::RectangleShape nodeSquare;
		nodeSquare.setSize(sf::Vector2f(tileWidth / 2, tileHeight / 2));

		// Draw connections to neighbours
		for (int x = 0; x < mapWidth; ++x)
		{
			for (int y = 0; y < mapHeight; ++y)
			{
				for (auto n : aStar->getNodes()[y * mapWidth + x].neighbours)
				{
					sf::Vertex connection[] =
					{
						sf::Vertex(sf::Vector2f(x * tileWidth + (tileWidth / 2), y * tileHeight + (tileHeight / 2))),
						sf::Vertex(sf::Vector2f(n->x * tileWidth + (tileWidth / 2), n->y * tileHeight + (tileHeight / 2)))
					};

					connection[0].color = sf::Color(connectionColour.x * 255, connectionColour.y * 255, connectionColour.z * 255);
					connection[1].color = sf::Color(connectionColour.x * 255, connectionColour.y * 255, connectionColour.z * 255);

					debugMap_RT.draw(connection, 2, sf::Lines);
				}
			}
		}

		// Draw the path by beginning at the end and following the parent node trail
		// back to the start - the start node will not have a parent path to follow
		if (aStar->getNodeEnd() != nullptr)
		{
			Node *p = aStar->getNodeEnd();

			while (p->parent != nullptr)
			{
				sf::Vertex connection[] =
				{
					sf::Vertex(sf::Vector2f(p->x * tileWidth + (tileWidth / 2), p->y * tileHeight + (tileHeight / 2))),
					sf::Vertex(sf::Vector2f(p->parent->x * tileWidth + (tileWidth / 2), p->parent->y * tileHeight + (tileHeight / 2)))
				};

				connection[0].color = sf::Color(pathColour.x * 255, pathColour.y * 255, pathColour.z * 255);
				connection[1].color = sf::Color(pathColour.x * 255, pathColour.y * 255, pathColour.z * 255);

				debugMap_RT.draw(connection, 2, sf::Lines);

				// Set next node to this node's parent
				p = p->parent;
			}
		}

		// Draw nodes
		for (int x = 0; x < mapWidth; ++x)
		{
			for (int y = 0; y < mapHeight; ++y)
			{
				nodeSquare.setFillColor(sf::Color(nodeColour.x * 255, nodeColour.y * 255, nodeColour.z * 255));

				if (aStar->getNodes()[y * mapWidth + x].obstacle == true) // Node is an obstacle
				{
					nodeSquare.setFillColor(sf::Color::Red);
				}

				if (aStar->getNodes()[y * mapWidth + x].visited == true) // Node was visited
				{
					nodeSquare.setFillColor(sf::Color(52, 171, 235));
				}

				if (&aStar->getNodes()[y * mapWidth + x] == aStar->getNodeStart()) // Node is start of path
				{
					nodeSquare.setFillColor(sf::Color::Green);
				}

				if (&aStar->getNodes()[y * mapWidth + x] == aStar->getNodeEnd()) // Node is end of path
				{
					nodeSquare.setFillColor(sf::Color(235, 131, 52));
				}

				nodeSquare.setPosition((x * tileWidth) + (tileWidth / 4), (y * tileHeight) + (tileWidth / 4));
				debugMap_RT.draw(nodeSquare);
			}
		}

		debugMap_RT.display();
	}	
}
