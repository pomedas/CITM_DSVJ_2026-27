#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name = "item";
}

Item::~Item() {}

bool Item::Awake() {
	return true;
}

bool Item::Start() {
	texture = Engine::GetInstance().textures->Load("Assets/Textures/goldCoin.png");
	Engine::GetInstance().textures->GetSize(texture, texW, texH);

	// L08: TODO 4: Create the item's physics body
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW / 2, bodyType::DYNAMIC);

	// L08: TODO 3: Assign collider type
	pbody->ctype = ColliderType::ITEM;

	// So Physics::BeginContact()/EndContact() can call back here
	pbody->listener = this;

	return true;
}

bool Item::Update(float dt)
{
	// L08: TODO 4: Sync the item's position from its physics body
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	return true;
}

bool Item::Draw()
{
	// L08: TODO 4: Once pbody exists, position is the body's CENTER -- offset
	// by half the sprite size when drawing
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX() - texW / 2, (int)position.getY() - texH / 2);
	return true;
}

bool Item::CleanUp()
{
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Item::Destroy()
{
	LOG("Destroying item");
	active = false;
	pendingToDelete = true;
	return true;
}
