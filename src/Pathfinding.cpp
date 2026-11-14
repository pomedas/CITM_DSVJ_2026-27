#include "Pathfinding.h"
#include "Engine.h"
#include "Textures.h"
#include "Map.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"

Pathfinding::Pathfinding() {
    
     //Loads texture to draw the path
    pathTex = Engine::GetInstance().textures.get()->Load("Assets/Maps/MapMetadata.png");

    // L12: texture used to draw the computed path
    tileX = Engine::GetInstance().textures.get()->Load("Assets/Maps/x.png");
    map = Engine::GetInstance().map.get();
    layerNav = map->GetNavigationLayer();

    //Creates a vector of the size of the navigation grid
    Vector2D mapSize = map->GetMapSizeInTiles();
    visited = std::vector<std::vector<bool>>((int)mapSize.getX(), std::vector<bool>((int)mapSize.getY(), false));

    // L12: breadcrumbs and costSoFar use the same layout as visited, so the three grids
    // are indexed the same way. They are created once here and cleared in ResetPath:
    // nothing is allocated during a search.
    breadcrumbs = std::vector<std::vector<Vector2D>>((int)mapSize.getX(), std::vector<Vector2D>((int)mapSize.getY(), Vector2D(-1, -1)));
    costSoFar = std::vector<std::vector<int>>((int)mapSize.getX(), std::vector<int>((int)mapSize.getY(), 0));
}

Pathfinding::~Pathfinding() {

}

bool Pathfinding::CleanUp()
{
    if (pathTex != nullptr) {
        Engine::GetInstance().textures->UnLoad(pathTex);
        pathTex = nullptr;
    }

    // L12
    if (tileX != nullptr) {
        Engine::GetInstance().textures->UnLoad(tileX);
        tileX = nullptr;
    }
    return true;
}

// L11: BFS Pathfinding methods
void Pathfinding::ResetPath(Vector2D pos) {

    // Clear the queue
    while (!frontier.empty()) {
        frontier.pop();
    }

    // L12: clear the Dijkstra frontier as well
    while (!frontierDijkstra.empty()) {
        frontierDijkstra.pop();
    }

    // L13: clear the A* frontier
    while (!frontierAStar.empty()) {
        frontierAStar.pop();
    }

    //Clear the visited list
    Vector2D mapSize = map->GetMapSizeInTiles();
    for (int i = 0; i < mapSize.getX(); i++){
        for (int j = 0; j < mapSize.getY(); j++) {
            visited[i][j] = false;

            // L12: clear the breadcrumbs and the accumulated costs
            breadcrumbs[i][j] = Vector2D(-1, -1);
            costSoFar[i][j] = 0;
        }
    }

    // L12: clear the path from the previous search
    pathTiles.clear();

    if (!IsWalkable((int)pos.getX(), (int)pos.getY()))
    {
        LOG("Pathfinding::ResetPath: start tile (%d,%d) is not walkable or out of bounds",
            (int)pos.getX(), (int)pos.getY());
        return;
    }

	// Add the starting position to the queue and mark it as visited
    visited[(int)pos.getX()][(int)pos.getY()] = true;
    frontier.push(pos);

    // L12: the start tile is its own breadcrumb. ComputePath walks the breadcrumbs
    // backwards and stops when a tile points to itself, so this is what ends the walk.
    breadcrumbs[(int)pos.getX()][(int)pos.getY()] = pos;

    // L12: the start costs nothing to reach, and seeds the Dijkstra frontier
    costSoFar[(int)pos.getX()][(int)pos.getY()] = 0;
    frontierDijkstra.push(std::make_pair(0, pos));
    frontierAStar.push(std::make_pair(0, pos));
}

void Pathfinding::DrawPath() {

    Vector2D point;
	Map* map = Engine::GetInstance().map.get();

    // Draw visited
    Vector2D mapSize = map->GetMapSizeInTiles();
    for (int i = 0; i < mapSize.getX(); i++){
        for (int j = 0; j < mapSize.getY(); j++) {
            if (visited[i][j]) {
                Vector2D pathTileWorld = Engine::GetInstance().map.get()->MapToWorld(i, j);

                // we use the second tile in the tileset to draw the visited. That's why we displace the size of 1 tile in the x axis of the rect and 1 piixel spacing 
                int posInTileSetX = map->GetTileWidth() + 1;
                int posInTileSetY = 1;

                SDL_Rect rect = { posInTileSetX,posInTileSetY,map->GetTileWidth(),map->GetTileHeight() };
                Engine::GetInstance().render->DrawTexture(pathTex, (int)pathTileWorld.getX(), (int)pathTileWorld.getY(), &rect);
            }
        }
    }

    // Draw frontier
    
    // Create a copy of the queue to iterate over
    std::queue<Vector2D> frontierCopy = frontier;

    // Iterate over the elements of the frontier copy
    while (!frontierCopy.empty()) {

        //Get the first element of the queue
        Vector2D frontierTile = frontierCopy.front();
        //Get the position of the frontier tile in the world
        Vector2D pos = Engine::GetInstance().map.get()->MapToWorld((int)frontierTile.getX(), (int)frontierTile.getY());

		//Draw the frontier tile. We use the first tile in the tileset to draw the frontier. 
        int posInTileSetX = 1;
        int posInTileSetY = 1;
        SDL_Rect rect = { posInTileSetX,posInTileSetY,map->GetTileWidth(),map->GetTileHeight() };
        Engine::GetInstance().render->DrawTexture(pathTex, (int)pos.getX(), (int)pos.getY(), &rect);

		//Draw additional tile in frontier if it is the player position.
        Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
        Vector2D playerPosTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());
        if (frontierTile == playerPosTile) {
            Engine::GetInstance().render->DrawTexture(pathTex, (int)pos.getX(), (int)pos.getY(), &rect);
        }

        //Remove the front element from the queue
        frontierCopy.pop();
    }

    // L12: Draw the Dijkstra frontier, same idea as the BFS one.
    // A priority queue cannot be iterated either, so we copy it and empty the copy.
    std::priority_queue<std::pair<int, Vector2D>, 
                        std::vector<std::pair<int, Vector2D>>, 
                        std::greater<std::pair<int, Vector2D>>> frontierDijkstraCopy = frontierDijkstra;


    while (!frontierDijkstraCopy.empty()) {

        Vector2D frontierTile = frontierDijkstraCopy.top().second;
        Vector2D pos = map->MapToWorld((int)frontierTile.getX(), (int)frontierTile.getY());

        int posInTileSetX = 1;
        int posInTileSetY = 1;
        SDL_Rect rect = { posInTileSetX,posInTileSetY,map->GetTileWidth(),map->GetTileHeight() };
        Engine::GetInstance().render->DrawTexture(pathTex, (int)pos.getX(), (int)pos.getY(), &rect);

        frontierDijkstraCopy.pop();
    }

    // L13: Draw the A* frontier
    std::priority_queue<std::pair<int, Vector2D>,
                        std::vector<std::pair<int, Vector2D>>,
                        std::greater<std::pair<int, Vector2D>>> frontierAStarCopy = frontierAStar;

    while (!frontierAStarCopy.empty()) {

        Vector2D frontierTile = frontierAStarCopy.top().second;
        Vector2D pos = map->MapToWorld((int)frontierTile.getX(), (int)frontierTile.getY());

        int posInTileSetX = 1;
        int posInTileSetY = 1;
        SDL_Rect rect = { posInTileSetX,posInTileSetY,map->GetTileWidth(),map->GetTileHeight() };
        Engine::GetInstance().render->DrawTexture(pathTex, (int)pos.getX(), (int)pos.getY(), &rect);

        frontierAStarCopy.pop();
    }

    // L12: Draw the computed path. x.png is a single 32x32 image, not a tileset,
    // so it is drawn whole instead of with a source rect.
    for (const auto& pathTile : pathTiles) {
        Vector2D pos = map->MapToWorld((int)pathTile.getX(), (int)pathTile.getY());
        Engine::GetInstance().render->DrawTexture(tileX, (int)pos.getX(), (int)pos.getY());
    }

}

bool Pathfinding::IsWalkable(int x, int y) {

    bool isWalkable = false;

    // L11: TODO 3: return true only if x and y are within map limits
    // and the tile is walkable (not blocked)
    if(layerNav != nullptr) {

		//Check map limits
        if (x >= 0 && x < map->GetMapSizeInTiles().getX() &&
            y >= 0 && y < map->GetMapSizeInTiles().getY()) {
			//Get the gid of the tile
            int gid = layerNav->Get(x, y);

			//Check if the gid is different from the blocked gid
            if (gid != blockedGid) {
                isWalkable = true;
            }
        }
	}

    return isWalkable;
}

void Pathfinding::PropagateBFS() {

    // L11 TODO 4: Check if we have reach a destination
    bool foundDestination = false;
    if (!frontier.empty()) {
        Vector2D frontierTile = frontier.front();
		Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
        Vector2D playerPosTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());

        if (frontierTile == playerPosTile) {
            foundDestination = true;

            // L12: TODO 2: the destination is reached, follow the breadcrumbs back to the start
            ComputePath(frontierTile);
        }
    }

    // L11: TODO 1: If frontier queue contains elements pop the first element and find the neighbors
    if (!frontier.empty() && !foundDestination) {
        //Get the value of the firt element in the queue
        Vector2D frontierTile = frontier.front();
        //remove the first element from the queue
        frontier.pop();
		//Get the neighbors (4 directions)
		std::list<Vector2D> neighbors;
        if (IsWalkable((int)frontierTile.getX() + 1, (int)frontierTile.getY())) {
            neighbors.push_back(Vector2D(frontierTile.getX() + 1, frontierTile.getY()));
        }
        if (IsWalkable((int)frontierTile.getX(), (int)frontierTile.getY() + 1)) {
            neighbors.push_back(Vector2D(frontierTile.getX(), frontierTile.getY() + 1));
        }
        if (IsWalkable((int)frontierTile.getX() - 1, (int)frontierTile.getY())) {
            neighbors.push_back(Vector2D(frontierTile.getX() - 1, frontierTile.getY()));
        }
        if (IsWalkable((int)frontierTile.getX(), (int)frontierTile.getY() - 1)) {
            neighbors.push_back(Vector2D(frontierTile.getX(), frontierTile.getY() - 1));
        }

        // L11: TODO 2: For each neighbor, if not visited, add it to the frontier queue and visited list
        for (const auto& neighbor : neighbors) {

            if (!visited[(int)neighbor.getX()][(int)neighbor.getY()]) {
                visited[(int)neighbor.getX()][(int)neighbor.getY()] = true;

                // L12: TODO 1: record the tile we reached this neighbour from
                breadcrumbs[(int)neighbor.getX()][(int)neighbor.getY()] = frontierTile;
                frontier.push(neighbor);
            }
        }

    }
}

// L12: Follow the breadcrumbs from the destination back to the start.
void Pathfinding::ComputePath(Vector2D destinationTile) {

    // L12: TODO 2: Follow the breadcrumps to goal back to the origin
    // at each step, add the point into "pathTiles" (it will then draw automatically)

    // Clear the previous path
    pathTiles.clear();
    // Add the destination tile to the path
    pathTiles.push_back(destinationTile);
    // Start from the destination tile and follow the breadcrumbs back to the start
    Vector2D currentTile = destinationTile;

    //follow the breadcrumbs back to the start, stopping when a tile points to itself
    while (breadcrumbs[(int)currentTile.getX()][(int)currentTile.getY()] != currentTile) {
        currentTile = breadcrumbs[(int)currentTile.getX()][(int)currentTile.getY()];
        pathTiles.push_back(currentTile);
    }
}

// L12: Cost of entering a tile.
// This is only called on walkable tiles: checking the map limits and the blocked tiles
// is IsWalkable's job. It must NEVER return a negative value, because the result is
// added to the accumulated cost of the path.
int Pathfinding::MovementCost(int x, int y) {

    if (layerNav == nullptr) return 1;
    if (layerNav->Get(x, y) == highCostGid) return highCost;

    return 1;
}

void Pathfinding::PropagateDijkstra() {

    // L12: TODO 3: Dijkstra = BFS + movement cost + priority queue.
    // Note that the guard is on frontierDijkstra, the queue we are about to read.
    bool foundDestination = false;
    if (!frontierDijkstra.empty()) {
        Vector2D frontierTile = frontierDijkstra.top().second;
        Vector2D playerPos = Engine::GetInstance().scene->GetPlayerPosition();
        Vector2D playerPosTile = Engine::GetInstance().map->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());

        if (frontierTile == playerPosTile) {
            foundDestination = true;
            ComputePath(frontierTile);
        }
    }

    if (!frontierDijkstra.empty() && !foundDestination) {

        //Get the cheapest tile in the queue, not the oldest one
        Vector2D frontierTile = frontierDijkstra.top().second;
        //remove it from the queue
        frontierDijkstra.pop();

        //Get the neighbors (4 directions)
        std::list<Vector2D> neighbors;
        if (IsWalkable((int)frontierTile.getX() + 1, (int)frontierTile.getY())) {
            neighbors.push_back(Vector2D(frontierTile.getX() + 1, frontierTile.getY()));
        }
        if (IsWalkable((int)frontierTile.getX(), (int)frontierTile.getY() + 1)) {
            neighbors.push_back(Vector2D(frontierTile.getX(), frontierTile.getY() + 1));
        }
        if (IsWalkable((int)frontierTile.getX() - 1, (int)frontierTile.getY())) {
            neighbors.push_back(Vector2D(frontierTile.getX() - 1, frontierTile.getY()));
        }
        if (IsWalkable((int)frontierTile.getX(), (int)frontierTile.getY() - 1)) {
            neighbors.push_back(Vector2D(frontierTile.getX(), frontierTile.getY() - 1));
        }

        for (const auto& neighbor : neighbors) {

            //Cost of reaching this neighbour through the tile we just took out
            int newCost = costSoFar[(int)frontierTile.getX()][(int)frontierTile.getY()] + MovementCost((int)neighbor.getX(), (int)neighbor.getY());

            //Take it if we have never reached it, or if we have just found a cheaper way in.
            //The second case is what BFS never does: a tile can be re-visited.
            if (!visited[(int)neighbor.getX()][(int)neighbor.getY()] || newCost < costSoFar[(int)neighbor.getX()][(int)neighbor.getY()]) {

                visited[(int)neighbor.getX()][(int)neighbor.getY()] = true;
                costSoFar[(int)neighbor.getX()][(int)neighbor.getY()] = newCost;
                breadcrumbs[(int)neighbor.getX()][(int)neighbor.getY()] = frontierTile;
                frontierDijkstra.push(std::make_pair(newCost, neighbor));
            }
        }

    }
}

void Pathfinding::PropagateAStar(ASTAR_HEURISTICS heuristic) {

    // L13: TODO 2: Adapt Dijkstra algorithm for AStar. Consider the different heuristics

}
