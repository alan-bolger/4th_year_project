#include "TileMap.h"

/// <summary>
/// TileMap constructor.
/// </summary>
TileMap::TileMap()
{

}

/// <summary>
/// TileMap destructor.
/// </summary>
TileMap::~TileMap()
{

}

/// <summary>
/// Returns a pointer to the tile array.
/// </summary>
/// <returns>A pointer to the array that stores the map data / tile ID numbers.</returns>
std::vector<int> *TileMap::getTileArray()
{
	return &tileArr;
}

/// <summary>
/// Load a map from a file.
/// Files must be in CSV format.
/// </summary>
/// <param name="fileName">The name (including path) of the file.</param>
/// <param name="mapW">The map width (in tiles).</param>
/// <param name="mapH">The map height (in tiles).</param>
void TileMap::loadTileMap(const std::string &fileName, sf::Texture *tileSet, const unsigned int &tileW, const unsigned int &tileH, const unsigned int &mapW, const unsigned int &mapH)
{
	const int arrSize = mapW * mapH;
	tileArr.resize(arrSize);

	std::ifstream data(fileName);
	std::string line;
	int counter = 0;

	while (std::getline(data, line, ',') && counter < arrSize)
	{
		tileArr[counter] = std::atoi(line.c_str());
		counter++;
	}

	// Create array for vertices
	vertices.setPrimitiveType(sf::Quads);
	vertices.resize(mapW * mapH * 4);

	// These are used in the draw function
	this->tileSet = tileSet;
	this->tileW = tileW;
	this->tileH = tileH;
	this->mapW = mapW;
	this->mapH = mapH;
}

/// <summary>
/// Draw the tile map.
/// This only draws the portion of the tile map that's currently
/// visible on-screen. It does not render any non-visible tiles.
/// </summary>
/// <param name="target">A render target.</param>
/// <param name="zoom">The current zoom level.</param>
/// <param name="alpha">The opacity of the tilemap.</param>
void TileMap::draw(sf::RenderTarget &target, float zoom, const uint8_t &alpha)
{
	// Optimise tile drawing
	int startTileX = (target.getView().getCenter().x - (target.getSize().x / 2) * zoom) / tileW;
	int startTileY = (target.getView().getCenter().y - (target.getSize().y / 2) * zoom) / tileH;

	int endTileX = (target.getView().getCenter().x + (target.getSize().x / 2) * zoom) / tileW;
	int endTileY = (target.getView().getCenter().y + (target.getSize().y / 2) * zoom) / tileH;

	// This dictates how many tiles outside of the screen view will be drawn (used to avoid artifacts around the screen borders)
	const int safeZone = 2;

	// Create quads for tiles
	for (int y = startTileY - safeZone; y < endTileY + safeZone; ++y)
	{
		for (int x = startTileX - safeZone; x < endTileX + safeZone; ++x)
		{
			if (y < 0 || y >= mapH || x < 0 || x >= mapW)
			{
				continue;
			}

			tileNumber = tileArr[y * mapW + x] - 1; // The negative 1 is a 'Tiled' map export thing

			if (tileNumber == -1)
			{
				continue;
			}

			// Get the UV coordinates of the tile in the tileset
			tu = tileNumber % (tileSet->getSize().x / tileW);
			tv = tileNumber / (tileSet->getSize().x / tileW);

			// Tile vertex coordinates
			auto topLeft = sf::Vector2i((x * tileW), (y * tileH));
			auto topRight = sf::Vector2i((x * tileW) + tileW, (y * tileH));
			auto bottomRight = sf::Vector2i((x * tileW) + tileW, (y * tileH) + tileH);
			auto bottomLeft = sf::Vector2i((x * tileW), (y * tileH) + tileH);

			// UV coordinates
			auto uvTopLeft = sf::Vector2f(tu * tileW, tv * tileH);
			auto uvTopRight = sf::Vector2f((tu + 1) * tileW, tv * tileH);
			auto uvBottomRight = sf::Vector2f((tu + 1) * tileW, (tv + 1) * tileH);
			auto uvBottomLeft = sf::Vector2f(tu * tileW, (tv + 1) * tileH);

			// Create the vertices
			sf::Vertex vertices[] =
			{
				sf::Vertex(sf::Vector2f(topLeft), uvTopLeft),
				sf::Vertex(sf::Vector2f(topRight), uvTopRight),
				sf::Vertex(sf::Vector2f(bottomRight), uvBottomRight),
				sf::Vertex(sf::Vector2f(bottomLeft), uvBottomLeft),
			};

			sf::RenderStates state;
			state.texture = tileSet;

			target.draw(vertices, 4, sf::Quads, state);
		}
	}

	//renderTexture.display();

	//sf::Sprite renderSprite(renderTexture.getTexture());
	//renderSprite.setColor(sf::Color(renderSprite.getColor().r, renderSprite.getColor().g, renderSprite.getColor().b, alpha));

	//target.draw(renderSprite);
}