#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Input.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "Audio.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player()
{}

bool Player::Awake()
{
	// L10: initial position stays in code (see TODO 3) -- it's map data,
	// not player config, and L15 loads it from the map instead. Away from
	// the left wall (L09's Collisions layer).
	// L04: TODO 2: Initialize the player's position
	position = Vector2D(300.0f, 300.0f);
	return true;
}

bool Player::Start()
{
	// L10: TODO 3: Read texture, frame size and tuning from config.xml
	// instead of hardcoding them. GetSize() is not the fix for texW/texH --
	// this texture is a spritesheet, so it would return the whole sheet's
	// size, not one frame's.
	std::string texturePath = parameters.child("texture").attribute("path").as_string("Assets/Textures/player2_spritesheet.png");
	// L04: TODO 2: Load the player texture
	texture = Engine::GetInstance().textures->Load(texturePath.c_str());

	texW = parameters.child("frame").attribute("width").as_int(32);
	texH = parameters.child("frame").attribute("height").as_int(32);

	speed = parameters.child("movement").attribute("speed").as_float(speed);

	// L10: TODO 4: Load this entity's animations from the TSX Tiled wrote.
	// player2_spritesheet's own layout: tile 0 starts "idle", 11 starts
	// "move" -- these clip names come from the sheet's own layout, so they
	// stay in code rather than moving to config.xml.
	std::string tsxPath = parameters.child("animations").attribute("tsx").as_string("Assets/Textures/player2_spritesheet.tsx");
	anims.LoadFromTSX(tsxPath.c_str(), { {0, "idle"}, {11, "move"} });
	anims.SetCurrent("idle");

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
	// L10: TODO 6: Default to the "idle" animation; a key below overrides it
	anims.SetCurrent("idle");

	// Move left/right
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -speed;
		// L10: TODO 6: Switch to the "move" animation
		anims.SetCurrent("move");
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = speed;
		// L10: TODO 6: Switch to the "move" animation
		anims.SetCurrent("move");
	}

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);

	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	// L10: TODO 8: Camera follow
	UpdateCamera();

	// L10: TODO 5: Advance the animation clock -- logic, so it belongs in
	// Update(), not Draw() (the L04 boundary: Update() is logic, Draw() is
	// rendering)
	anims.Update(dt);

	return true;
}

void Player::UpdateCamera()
{
	// L10: TODO 8: Horizontal-only follow (no vertical -- jumping would
	// otherwise fight a Y follow), quarter mark rather than dead centre:
	// the classic side-scroller composition, showing more of what's ahead
	// than behind. Writes render->camera.x directly -- it is a float
	// (SDL_FRect since L01) and Render rounds at draw time, so there is no
	// truncation to guard against here.
	Vector2D mapSize = Engine::GetInstance().map->GetMapSizeInPixels();
	float camW = Engine::GetInstance().render->camera.w;
	float limitLeft = camW / 4.0f;
	float limitRight = mapSize.getX() - camW * 3.0f / 4.0f;

	if (position.getX() - limitLeft > 0 && position.getX() < limitRight) {
		Engine::GetInstance().render->camera.x = camW / 4.0f - position.getX();
	}
}

bool Player::Draw()
{
	// L08: TODO 5: Add physics to the player - updated player position using physics

	// L10: TODO 7: Draw the current animation frame instead of the whole
	// spritesheet -- rendering, so it belongs in Draw(), not Update()
	const SDL_Rect& animFrame = anims.GetCurrentFrame();
	// L04: TODO 4: Render the player texture at its current position
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2, &animFrame);

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
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
		Engine::GetInstance().audio->PlayFx(pickCoinFxId);
		//physB->listener->Destroy();
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}