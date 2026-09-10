#pragma once

#include "Entity.h"
#include <SDL3/SDL.h>

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();

	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool Draw();

	bool CleanUp();

	// L08: TODO 2: React to collisions -- reset the jump flag on the ground,
	// play a sound and destroy the item on pickup
	// ...

public:

	// Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;

	// L08: TODO 1: Add a physics body to the player
	// ...
};
