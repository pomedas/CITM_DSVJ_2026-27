#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"
#include "Scene.h"
#include "Map.h"
#include "Window.h"

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
	// the left wall (L09's Collisions layer): spawning at the old (64, 64)
	// sat the capsule right against it.
	position = Vector2D(300, 300);
	return true;
}

bool Player::Start()
{
	// L10: TODO 3: Read texture, frame size and tuning from config.xml
	// instead of hardcoding them. GetSize() is not the fix for texW/texH --
	// this texture is a spritesheet, so it would return the whole sheet's
	// size, not one frame's.
	std::string texturePath = parameters.child("texture").attribute("path").as_string("Assets/Textures/player2_spritesheet.png");
	texture = Engine::GetInstance().textures->Load(texturePath.c_str());

	texW = parameters.child("frame").attribute("width").as_int(32);
	texH = parameters.child("frame").attribute("height").as_int(32);

	speed = parameters.child("movement").attribute("speed").as_float(speed);
	jumpForce = parameters.child("movement").attribute("jumpForce").as_float(jumpForce);

	// L10: TODO 4: Load this entity's animations from the TSX Tiled wrote.
	// player2_spritesheet's own layout: tile 0 starts "idle", 11 starts
	// "move", 22 starts "jump" -- these clip names come from the sheet's
	// own layout, so they stay in code rather than moving to config.xml.
	std::string tsxPath = parameters.child("animations").attribute("tsx").as_string("Assets/Textures/player2_spritesheet.tsx");
	anims.LoadFromTSX(tsxPath.c_str(), { {0, "idle"}, {11, "move"}, {22, "jump"} });
	anims.SetCurrent("idle");

	// L08: TODO 1: Create the player's physics body -- a capsule, not a
	// circle, with fixed rotation so it doesn't roll. width == height for
	// this sprite, so the capsule geometrically degenerates to a circle;
	// fixedRotation is where the actual improvement over L07 comes from.
	pbody = Engine::GetInstance().physics->CreateCapsule((int)position.getX(), (int)position.getY(), texW, texH, bodyType::DYNAMIC);

	// L08: TODO 2: so Physics::BeginContact()/EndContact() can call back here
	pbody->listener = this;

	// L08: TODO 3: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	// Initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/coin-collision-sound-342335.wav");

	return true;
}

bool Player::Update(float dt)
{
	GetPhysicsValues();
	Move();
	Jump();
	ApplyPhysics();
	UpdatePosition();
	UpdateCamera();

	// L10: TODO 5: Advance the animation clock -- logic, so it belongs in
	// Update(), not Draw() (the L04 boundary: Update() is logic, Draw() is
	// rendering; this is exactly the split the old course lost by L19)
	anims.Update(dt);

	return true;
}

void Player::GetPhysicsValues() {
	// Read current velocity
	velocity = Engine::GetInstance().physics->GetLinearVelocity(pbody);
	velocity = { 0, velocity.y }; // Reset horizontal velocity by default, this way the player stops when no key is pressed
}

void Player::Move() {
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
}

void Player::Jump() {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
		// L10: TODO 6: Switch to the "jump" animation
		anims.SetCurrent("jump");
		isJumping = true;
	}
}

void Player::ApplyPhysics() {
	// Preserve vertical speed while jumping
	if (isJumping == true) {
		velocity.y = Engine::GetInstance().physics->GetYVelocity(pbody);
	}

	// Apply velocity via helper
	Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity);
}

void Player::UpdatePosition() {
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);
}

void Player::UpdateCamera() {
	// L10: TODO 8: Horizontal-only follow (no vertical -- jumping would
	// otherwise fight a Y follow), quarter mark rather than dead centre:
	// the classic side-scroller composition, showing more of what's ahead
	// than behind. Routed through Scene::SetCameraX() -- the one place that
	// casts to int and writes render->camera (L03) -- never render->camera.x
	// directly, which would reintroduce the truncation L03 removed.
	Vector2D mapSize = Engine::GetInstance().map->GetMapSizeInPixels();
	float camW = (float)Engine::GetInstance().render->camera.w;
	float limitLeft = camW / 4.0f;
	float limitRight = mapSize.getX() - camW * 3.0f / 4.0f;

	if (position.getX() - limitLeft > 0 && position.getX() < limitRight) {
		Engine::GetInstance().scene->SetCameraX(camW / 4.0f - position.getX());
	}
}

bool Player::Draw() {
	// L08: TODO 1: Once pbody exists, position is the body's CENTER -- offset
	// by half the sprite size when drawing
	// L10: TODO 7: Draw the current animation frame instead of the whole
	// spritesheet -- rendering, so it belongs in Draw(), not Update()
	const SDL_Rect& animFrame = anims.GetCurrentFrame();
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2, &animFrame);
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}

// L08: TODO 2: React to collisions
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		// Reset the jump flag when touching the ground
		isJumping = false;
		// L10: TODO 6: Back to "idle" on landing
		anims.SetCurrent("idle");
		break;
	case ColliderType::ITEM:
		Engine::GetInstance().audio->PlayFx(pickCoinFxId);
		physB->listener->Destroy();
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}
