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
	position = Vector2D(100.0f, 100.0f);
	return true;
}

bool Player::Start()
{
	// L04: TODO 2: Load the player texture
	texture = Engine::GetInstance().textures->Load("Assets/Textures/player1.png");
	Engine::GetInstance().textures->GetSize(texture, texW, texH);

	// L08: TODO 5: Add physics to the player - create the physics body: a CAPSULE (CreateCapsule) with fixed rotation, so the player does not roll

	// L08: TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method

	// L08: TODO 7: Assign collider type

	return true;
}

bool Player::Update(float dt)
{
	// L08: TODO 5: Add physics to the player - updated player position using physics

	// L04: TODO 5: Move the player with WASD, independent of framerate
	float speed = 200.0f;

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		position.setY(position.getY() - speed * dt);

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		position.setY(position.getY() + speed * dt);

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		position.setX(position.getX() - speed * dt);

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		position.setX(position.getX() + speed * dt);

	return true;
}

bool Player::Draw()
{
	// L08: TODO 5: Add physics to the player - updated player position using physics

	// L04: TODO 4: Render the player texture at its current position
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX(), (int)position.getY());
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");

	// Release the physics body here before unloading the texture

	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}

// L08: TODO 6: Define OnCollision function for the player.
