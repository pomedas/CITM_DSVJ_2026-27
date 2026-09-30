#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"

#include <iterator>

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
		for (const auto& tileset : mapData.tilesets) {
			Engine::GetInstance().render->DrawTexture(tileset.texture, 0, 0);
		}
	}

	return ret;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	// L06: TODO 2: Unload every tileset's texture
	for (const auto& tileset : mapData.tilesets) {
		Engine::GetInstance().textures->UnLoad(tileset.texture);
	}
	mapData.tilesets.clear();

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
		pugi::xml_node mapNode = mapFileXML.child("map");
		mapData.width = mapNode.attribute("width").as_int();
		mapData.height = mapNode.attribute("height").as_int();
		mapData.tileWidth = mapNode.attribute("tilewidth").as_int();
		mapData.tileHeight = mapNode.attribute("tileheight").as_int();

		// L06: TODO 4: Load every <tileset>, including its image; a missing
		// image fails the whole load instead of silently drawing nothing later
		ret = true;

		mapData.tilesets.reserve(std::distance(mapNode.children("tileset").begin(), mapNode.children("tileset").end()));

		for (pugi::xml_node tilesetNode : mapNode.children("tileset"))
		{
			TileSet tileSet;
			tileSet.firstGid = tilesetNode.attribute("firstgid").as_int();
			tileSet.name = tilesetNode.attribute("name").as_string();
			tileSet.tileWidth = tilesetNode.attribute("tilewidth").as_int();
			tileSet.tileHeight = tilesetNode.attribute("tileheight").as_int();
			tileSet.spacing = tilesetNode.attribute("spacing").as_int();
			tileSet.margin = tilesetNode.attribute("margin").as_int();
			tileSet.tileCount = tilesetNode.attribute("tilecount").as_int();
			tileSet.columns = tilesetNode.attribute("columns").as_int();

			std::string imgName = tilesetNode.child("image").attribute("source").as_string();
			tileSet.texture = Engine::GetInstance().textures->Load((mapPath + imgName).c_str());

			if (tileSet.texture == NULL) {
				LOG("Could not load tileset image %s", (mapPath + imgName).c_str());
				ret = false;
			}

			mapData.tilesets.push_back(tileSet);
		}

		// L06: TODO 5: LOG all the data loaded, iterating all tilesets
		if (ret == true)
		{
			LOG("Successfully parsed map XML file: %s", fileName.c_str());
			LOG("width : %d height : %d", mapData.width, mapData.height);
			LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);
			LOG("Tilesets----");

			for (const auto& tileset : mapData.tilesets) {
				LOG("name : %s firstgid : %d", tileset.name.c_str(), tileset.firstGid);
				LOG("tile width : %d tile height : %d", tileset.tileWidth, tileset.tileHeight);
				LOG("spacing : %d margin : %d", tileset.spacing, tileset.margin);
			}
		}
		else {
			LOG("Error while parsing map file: %s", mapPathName.c_str());
		}

		if (mapFileXML) mapFileXML.reset();
	}

	mapLoaded = ret;
	return ret;
}
