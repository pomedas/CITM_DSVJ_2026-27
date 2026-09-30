#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player()
{}

bool Player::Awake()
{
	position = Vector2D(16, 0);
	return true;
}

bool Player::Start()
{
	texture = Engine::GetInstance().textures->Load("Assets/Textures/player1.png");
	Engine::GetInstance().textures->GetSize(texture, texW, texH);

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
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = speed;
	}
}

void Player::Jump() {
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
		Engine::GetInstance().physics->ApplyLinearImpulseToCenter(pbody, 0.0f, -jumpForce, true);
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

bool Player::Draw() {
	// L08: TODO 1: Once pbody exists, position is the body's CENTER -- offset
	// by half the sprite size when drawing
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2);
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
