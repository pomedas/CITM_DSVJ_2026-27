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
	position = Vector2D(0, 0);
	return true;
}

bool Player::Start()
{
	// L04: TODO 3: Load the player texture
	texture = Engine::GetInstance().textures->Load("Assets/Textures/player1.png");
	return true;
}

bool Player::Update(float dt)
{
	// L04: TODO 6: Move the player with WASD, independent of framerate
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
	// L04: TODO 7: Render the player texture at its current position
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX(), (int)position.getY());
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}
