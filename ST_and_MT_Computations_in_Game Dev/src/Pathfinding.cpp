#include "Pathfinding.h"

/// <summary>
/// Pathfinding constructor.
/// </summary>
Pathfinding::Pathfinding() : window{ sf::VideoMode{ SCREEN_WIDTH, SCREEN_HEIGHT, 32 }, "Pathfinding", sf::Style::Default }
{
    aStar = new AStar(mapData, mapWidth, mapHeight);

	windowView.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	window.setView(windowView);

	// Quick path test
	aStar->run({ 3, 7 }, { 18, 15 });
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
    }
}

/// <summary>
/// Draws the map and shows connections, bots, paths, etc.
/// </summary>
void Pathfinding::render()
{
	// Clear render texture
	window.clear(sf::Color(0, 0, 0, 0));

	sf::RectangleShape nodeSquare;
	nodeSquare.setSize(sf::Vector2f(nodeWidth, nodeHeight));

	// Draw connections to neighbours
	for (int x = 0; x < mapWidth; ++x)
	{
		for (int y = 0; y < mapHeight; ++y)
		{
			for (auto n : aStar->getNodes()[y * mapWidth + x].neighbours)
			{
				sf::Vertex connection[] =
				{
					sf::Vertex(sf::Vector2f(x * nodeWidth * 2 + nodeWidth, y * nodeHeight * 2 + nodeHeight)),
					sf::Vertex(sf::Vector2f(n->x * nodeWidth * 2 + nodeWidth, n->y * nodeHeight * 2 + nodeHeight))
				};

				connection[0].color = sf::Color(connectionColour.x * 255, connectionColour.y * 255, connectionColour.z * 255);
				connection[1].color = sf::Color(connectionColour.x * 255, connectionColour.y * 255, connectionColour.z * 255);

				window.draw(connection, 2, sf::Lines);
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
				sf::Vertex(sf::Vector2f(p->x * nodeWidth * 2 + nodeWidth, p->y * nodeHeight * 2 + nodeHeight)),
				sf::Vertex(sf::Vector2f(p->parent->x * nodeWidth * 2 + nodeWidth, p->parent->y * nodeHeight * 2 + nodeHeight))
			};

			connection[0].color = sf::Color(pathColour.x * 255, pathColour.y * 255, pathColour.z * 255);
			connection[1].color = sf::Color(pathColour.x * 255, pathColour.y * 255, pathColour.z * 255);

			window.draw(connection, 2, sf::Lines);

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

			nodeSquare.setPosition(x * nodeWidth * 2 + borderSize, y * nodeHeight * 2 + borderSize);
			window.draw(nodeSquare);
		}
	}

	window.display();
}
