#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"
#include "Audio.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player()
{}

bool Player::Awake()
{
	// L04: TODO 2: Initialize the player's position
	position = Vector2D(300.0f, 300.0f);
	return true;
}

bool Player::Start()
{
	// L10: TODO 5: Read texture, frame size and tuning from config.xml
	// instead of hardcoding them. GetSize() is not the fix for texW/texH --
	// the real texture (a spritesheet) would return the whole sheet's size,
	// not one frame's.
	// L04: TODO 2: Load the player texture
	texture = Engine::GetInstance().textures->Load("Assets/Textures/player1.png");
	Engine::GetInstance().textures->GetSize(texture, texW, texH);

	// L10: TODO 5: Load this entity's animations from a TSX and set the current animation to "idle".
	// ...

	// Initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	// L08: TODO 5: Add physics to the player - create the physics body: a CAPSULE (CreateCapsule) with fixed rotation, so the player does not roll
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW / 2, bodyType::DYNAMIC);

	// L08: TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08: TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	return true;
}

bool Player::Update(float dt)
{
	// L08: TODO 5: Add physics to the player - updated player position using physics

	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0, velocity.y }; // Reset horizontal velocity by default, this way the player stops when no key is pressed

	// L10: TODO 7: Default to the "idle" animation; a key below overrides it
	// ..

	// Move left/right
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -speed;
		// L10: TODO 7: Switch to the "move" animation
		// ..
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = speed;
		// L10: TODO 7: Switch to the "move" animation
		// ..
	}

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);

	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	// L10: TODO 8: Camera follow
	// ..

	// L10: TODO 6: Advance the animation frame
	// ..

	return true;
}

bool Player::Draw()
{
	// L08: TODO 5: Add physics to the player - updated player position using physics
	// L04: TODO 4: Render the player texture at its current position

	// L10: TODO 6: Draw the current animation frame instead of the whole spritesheet
	// ..
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2);

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	// Release the physics body first, then the texture
	if (pbody != nullptr)
	{
		pbody->listener = nullptr;
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = nullptr;
	}

	if (texture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(texture);
		texture = nullptr;
	}
	return true;
}

// L08: TODO 6: Define OnCollision function for the player.
void Player::OnCollision(PhysBody* physA, PhysBody* physB)
{
	// Handle collision with other physics bodies
	// For example, if the player collides with an item, you can implement logic here
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		
		//Play the coin pickup sound effect 
		Engine::GetInstance().audio->PlayFx(pickCoinFxId);

		// Destroy the item. 
		physB->listener->Destroy(); 

		LOG("Collision ITEM");

		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}
