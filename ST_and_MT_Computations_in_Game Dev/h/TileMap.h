#ifndef TILEMAP_H
#define TILEMAP_H

#include <fstream>
#include <SFML/Graphics.hpp>

class TileMap
{
public:
	TileMap();
	~TileMap();
	std::vector<int> *getTileArray();
	void loadTileMap(const std::string &fileName, sf::Texture *tileSet, const unsigned int &tileW, const unsigned int &tileH, const unsigned int &mapW, const unsigned int &mapH);
	void draw(sf::RenderTarget &target, float zoom = 1.0f, const uint8_t &alpha = 255u);

private:
	sf::VertexArray vertices;
	sf::Texture *tileSet;
	sf::RenderStates state;
	int tileNumber;
	int tu;
	int tv;
	int tileW;
	int tileH;
	int mapW;
	int mapH;
	std::vector<int> tileArr;
};

#endif // !TILEMAP_H