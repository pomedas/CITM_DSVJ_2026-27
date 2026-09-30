#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"

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

		// L07: TODO 5: Draw every tile in every layer
		// L07: TODO 9: Get the gid, look up its rect in the tileset, convert
		// tile coordinates to world coordinates, then draw
		// L09: TODO 7: This always uses the first tileset -- once
		// GetTilesetFromTileId() exists, look the right one up per gid instead
		const TileSet& tileSet = mapData.tilesets.front();

		for (const auto& mapLayer : mapData.layers) {
			// Helper layers (e.g. the collision mask) are never meant to be
			// seen -- skip anything Tiled itself marked not visible.
			if (!mapLayer.visible) continue;

			for (int i = 0; i < mapData.width; i++) {
				for (int j = 0; j < mapData.height; j++) {

					unsigned int gid = mapLayer.Get(i, j);
					SDL_Rect tileRect = tileSet.GetRect(gid);
					Vector2D mapCoord = MapToWorld(i, j);

					Engine::GetInstance().render->DrawTexture(tileSet.texture, (int)mapCoord.getX(), (int)mapCoord.getY(), &tileRect);
				}
			}
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

	// L07: TODO 2: Clear the layers
	mapData.layers.clear();

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

		// L07: TODO 3: Load every <layer>
		// L07: TODO 4: Read one layer's attributes and its tile data
		mapData.layers.reserve(std::distance(mapNode.children("layer").begin(), mapNode.children("layer").end()));

		for (pugi::xml_node layerNode : mapNode.children("layer"))
		{
			MapLayer mapLayer;
			mapLayer.id = layerNode.attribute("id").as_int();
			mapLayer.name = layerNode.attribute("name").as_string();
			mapLayer.width = layerNode.attribute("width").as_int();
			mapLayer.height = layerNode.attribute("height").as_int();
			mapLayer.visible = layerNode.attribute("visible").as_bool(true);

			// L09: TODO 5: Load this layer's custom properties
			// ...

			mapLayer.tiles.reserve((size_t)mapLayer.width * mapLayer.height);
			for (pugi::xml_node tileNode : layerNode.child("data").children("tile"))
			{
				mapLayer.tiles.push_back(tileNode.attribute("gid").as_int());
			}

			mapData.layers.push_back(mapLayer);
		}

		// Temporary L08 placeholder: a few hand-placed platform colliders so
		// there is something for the player to stand and jump on.
		// L09: TODO 8: Replace this whole block with a loop that builds one
		// rectangle collider per non-empty tile, for any layer whose
		// "Collision" custom property is true (see the invisible
		// "Collisions" layer in MapTemplate.tmx) -- colliders derived from
		// the map's own tile data instead of hardcoded pixel rectangles.
		Vector2D posC1 = Vector2D(224, 544);
		int widthC1 = 256;
		int heightC1 = 64;
		PhysBody* c1 = Engine::GetInstance().physics->CreateRectangle((int)posC1.getX() + widthC1 / 2, (int)posC1.getY() + heightC1 / 2, widthC1, heightC1, bodyType::STATIC);
		c1->ctype = ColliderType::PLATFORM;

		Vector2D posC2 = Vector2D(352, 384);
		int widthC2 = 128;
		int heightC2 = 64;
		PhysBody* c2 = Engine::GetInstance().physics->CreateRectangle((int)posC2.getX() + widthC2 / 2, (int)posC2.getY() + heightC2 / 2, widthC2, heightC2, bodyType::STATIC);
		c2->ctype = ColliderType::PLATFORM;

		Vector2D posC3 = Vector2D(0, 704);
		int widthC3 = 544;
		int heightC3 = 64;
		PhysBody* c3 = Engine::GetInstance().physics->CreateRectangle((int)posC3.getX() + widthC3 / 2, (int)posC3.getY() + heightC3 / 2, widthC3, heightC3, bodyType::STATIC);
		c3->ctype = ColliderType::PLATFORM;

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

			// L07: TODO 3: LOG every layer too
			LOG("Layers----");
			for (const auto& layer : mapData.layers) {
				LOG("id : %d name : %s", layer.id, layer.name.c_str());
				LOG("Layer width : %d Layer height : %d", layer.width, layer.height);
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

// L07: TODO 8: Translate tile coordinates (i, j) into world (pixel) coordinates
Vector2D Map::MapToWorld(int x, int y) const
{
	Vector2D ret;

	ret.setX((float)(x * mapData.tileWidth));
	ret.setY((float)(y * mapData.tileHeight));

	return ret;
}

