#pragma once

#include "Module.h"
#include <string>
#include <vector>

struct SDL_Texture;

// L07: TODO 1: Create a struct to hold information for a single map layer
struct MapLayer
{
	// ...

	// L07: TODO 6: Return the gid at tile (i, j)
	// ...
};

// L06: TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
struct TileSet
{
	int firstGid;
	std::string name;
	int tileWidth;
	int tileHeight;
	int spacing;
	int margin;
	int tileCount;
	int columns;
	SDL_Texture* texture;

	// L07: TODO 7: Return the source rect for this gid within the tileset image
	// ...
};

// L06: TODO 1: Create a struct needed to hold the information of the <map> node
struct MapData
{
	int width;
	int height;
	int tileWidth;
	int tileHeight;
	std::vector<TileSet> tilesets;

	// L07: TODO 2: Add a vector of MapLayer to MapData
	// ...
};

class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before rendering, after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(std::string path, std::string mapFileName);

	// L07: TODO 8: Translate tile coordinates (i, j) into world (pixel) coordinates
	// ...

public:
	std::string mapFileName;
	std::string mapPath;

private:
	bool mapLoaded;

	// L06: TODO 1: Declare a MapData member to hold the parsed map
	MapData mapData;
};
