#pragma once

#include "Module.h"
#include <string>
#include <vector>

struct SDL_Texture;

// L06: TODO 1: Create a struct needed to hold the information of the <map> node
// ...

// L06: TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
// ...

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

public:
	std::string mapFileName;
	std::string mapPath;

private:
	bool mapLoaded;

	// L06: TODO 1: Declare a MapData member to hold the parsed map
	// ...
};
