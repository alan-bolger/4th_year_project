#include "Pathfinding.h"

/// <summary>
/// Pathfinding constructor.
/// </summary>
Pathfinding::Pathfinding()
{

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
        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::SeparatorText("Map Size");

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::InputInt("Width##01", &mapWidth);
        ImGui::InputInt("Height##02", &mapHeight);

        // Limit map size to 4096 x 4096
        mapWidth = std::clamp(mapWidth, 16, 4096);
        mapHeight = std::clamp(mapHeight, 16, 4096);

        // Cosmetic options
        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::SeparatorText("Node Size");

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::InputInt("Width##03", &nodeWidth);
        ImGui::InputInt("Height##04", &nodeHeight);

        // Limit node size to 32 x 32
        nodeWidth = std::clamp(nodeWidth, 4, 32);
        nodeHeight = std::clamp(nodeHeight, 4, 32);

        ImGui::Dummy(ImVec2(0.0f, 8.0f));
    }
}
