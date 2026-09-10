#pragma once

#include "Entity.h"
#include "Animation.h"
#include <box2d/box2d.h>
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
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

private:

	void GetPhysicsValues();
	void Move();
	void Jump();
	void ApplyPhysics();
	void UpdatePosition();

	// L10: TODO 8: Horizontal-only camera follow, routed through Scene's
	// float camera position -- never render->camera.x directly
	void UpdateCamera();

public:

	// Declare player parameters
	float speed = 4.0f;
	SDL_Texture* texture = NULL;

	int texW, texH;

	// Audio fx
	int pickCoinFxId;

	// L08: TODO 1: Add a physics body to the player
	PhysBody* pbody;
	float jumpForce = 2.5f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping

private:
	b2Vec2 velocity;

	// L10: TODO 4: Player's animation clips, loaded from a TSX in Start()
	AnimationSet anims;
};
