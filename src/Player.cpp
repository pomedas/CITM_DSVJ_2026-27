#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Log.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player()
{}

bool Player::Awake()
{
	// L04: TODO 2: Initialize the player's position
	// ...
	return true;
}

bool Player::Start()
{
	// L04: TODO 3: Load the player texture
	// ...
	return true;
}

bool Player::Update(float dt)
{
	// L04: TODO 6: Move the player with WASD, independent of framerate
	// ...
	return true;
}

bool Player::Draw()
{
	// L04: TODO 7: Render the player texture at its current position
	// ...
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}
