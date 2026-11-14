#pragma once
#include <list>
#include <queue>
#include "Vector2D.h"
#include <SDL3/SDL.h>
#include "Map.h"

enum class ASTAR_HEURISTICS {
    MANHATTAN,
    EUCLIDEAN,
    SQUARED
};

class Pathfinding
{

public:

	Pathfinding();

	~Pathfinding();

    bool CleanUp();

    // L11: BFS Pathfinding methods
    void ResetPath(Vector2D pos);
    void DrawPath();
    bool IsWalkable(int x, int y);
    void PropagateBFS();

    // L12: Path reconstruction and Dijkstra
    void ComputePath(Vector2D destinationTile);
    int  MovementCost(int x, int y);
    void PropagateDijkstra();

    // L13: A* Pathfinding methods
    void PropagateAStar(ASTAR_HEURISTICS heuristic);
    
public:

    // L11: BFS Pathfinding variables
    Map* map;
    const MapLayer* layerNav;
    std::queue<Vector2D> frontier;
    std::vector<std::vector<bool>> visited;   // visited[x][y]
    SDL_Texture* pathTex = nullptr;
    Vector2D destination;

    //Gid of the tiles that block the path - Important adjust this value to your map
    int blockedGid = 49;

    // L12: Dijkstra Pathfinding variables

    //list of tiles that form the path
    std::vector<std::vector<Vector2D>> breadcrumbs; 
    //matrix that stores the accumulated cost in the propagation of the Dijkstra algorithm
    std::vector<std::vector<int>> costSoFar; 
    // The frontier ordered by accumulated cost
    std::priority_queue<std::pair<int, Vector2D>,
                        std::vector<std::pair<int, Vector2D>>,
                        std::greater<std::pair<int, Vector2D>>> frontierDijkstra;

    // L13: A* Pathfinding variables
    // The frontier ordered by the priority f = g + h
    std::priority_queue<std::pair<int, Vector2D>,
                        std::vector<std::pair<int, Vector2D>>,
                        std::greater<std::pair<int, Vector2D>>> frontierAStar;
    // The path, from the destination back to the start
    std::vector<Vector2D> pathTiles;
	// Texture for the path tiles
    SDL_Texture* tileX = nullptr;

    //Gid of the tiles that are expensive to cross - Important adjust this value to your map
    int highCostGid = 50;
    int highCost = 5;
};

