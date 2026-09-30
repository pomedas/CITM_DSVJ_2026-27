#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"

Map::Map() : Module(), mapLoaded(false)
{
	name = "map";
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake()
{
	LOG("Loading Map Parser");

	return true;
}

bool Map::Start()
{
	return true;
}

bool Map::PostUpdate()
{
	bool ret = true;

	if (mapLoaded) {

		// L06: TODO 6: Iterate all tilesets and draw all their
		// images at 0,0 (you should have only one tileset for now)
		// ...
	}

	return ret;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	// L06: TODO 2: Unload every tileset's texture
	// ...

	return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
	bool ret = false;

	// Assigns the name of the map file and the path
	mapFileName = fileName;
	mapPath = path;
	std::string mapPathName = mapPath + mapFileName;

	pugi::xml_document mapFileXML;
	pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

	if (!result)
	{
		LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		ret = false;
	}
	else {

		// L06: TODO 3: Read the <map> node's attributes into mapData
		// ...

		// L06: TODO 4: Load every <tileset>, including its image; a missing
		// image fails the whole load instead of silently drawing nothing later
		// ...

		// L06: TODO 5: LOG all the data loaded, iterating all tilesets
		if (ret == true)
		{
			LOG("Successfully parsed map XML file: %s", fileName.c_str());
		}
		else {
			LOG("Error while parsing map file: %s", mapPathName.c_str());
		}

		if (mapFileXML) mapFileXML.reset();
	}

	mapLoaded = ret;
	return ret;
}
