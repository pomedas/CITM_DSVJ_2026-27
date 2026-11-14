#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Map.h"
#include "Window.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name = "Enemy";
}

Enemy::~Enemy() {

}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

	// Texture, frame size and tuning come from the <enemy> section of config.xml
	texture = Engine::GetInstance().textures->Load(parameters.child("texture").attribute("path").as_string());
	texW = parameters.child("frame").attribute("width").as_int();
	texH = parameters.child("frame").attribute("height").as_int();
	speed = parameters.child("movement").attribute("speed").as_float(speed);

	anims.LoadFromTSX(parameters.child("animations").attribute("tsx").as_string(), { {0, "idle"} });
	anims.SetCurrent("idle");

	// Capsule body with fixed rotation (CreateCapsule sets it), like the player
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW / 2, bodyType::DYNAMIC);

	//Assign enemy class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	//ssign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Initialize pathfinding
	pathfinding = std::make_shared<Pathfinding>();
	//Get the position of the enemy
	Vector2D pos = GetPosition();
	//Convert to tile coordinates
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY()+1);
	//Reset pathfinding
	pathfinding->ResetPath(tilePos);

	return true;
}

bool Enemy::Update(float dt)
{
	// Position comes from the physics body
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	// Advance the animation
	anims.Update(dt);

	PerformPathfinding();

	return true;
}

bool Enemy::Draw()
{
	SDL_Rect currentFrame = anims.GetCurrentFrame();
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2, &currentFrame);

	// L11: Draw pathfinding
	pathfinding->DrawPath();

	return true;
}

void Enemy::PerformPathfinding() {

	// Pathfinding testing inputs

	// L13: TODO 4: Move the enemy to the clicked tile and reset the path

	// Reset pathfinding with R key
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		//Get the position of the enemy
		Vector2D pos = GetPosition();
		//Convert to tile coordinates
		Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)pos.getX(), (int)pos.getY()+1);
		//Reset pathfinding
		pathfinding->ResetPath(tilePos);
	}

	// Propagate BFS with J key
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_DOWN) {
		pathfinding->PropagateBFS();
	}

	// Propagate BFS continuously with LShift + J
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateBFS();
	}

	// L12: Propagate Dijkstra with K key
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
		pathfinding->PropagateDijkstra();
	}

	// L12: Propagate Dijkstra continuously with LShift + K
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateDijkstra();
	}

	// L13: TODO 3: Add the key inputs to propagate the A* algorithm with different heuristics (Manhattan, Euclidean, Squared)

}

bool Enemy::CleanUp()
{
	LOG("Cleanup enemy");
	if (pathfinding != nullptr) pathfinding->CleanUp();
	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
	Engine::GetInstance().physics->SetLinearVelocity(pbody, b2Vec2_zero);
}

Vector2D Enemy::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x,(float)y);
}

//Define OnCollision function for the enemy. 
void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {

}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{

}
