#pragma once

#include "Module.h"
#include <memory>

class Player;
class Enemy;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// L11 Return the player position
	Vector2D GetPlayerPosition();

private:
	// L04: TODO 3a: Declare a player as a member of Scene.h
	std::shared_ptr<Player> player;

	// L11: 
	std::shared_ptr<Enemy> enemy;
};