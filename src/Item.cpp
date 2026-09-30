#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"

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

	// L08: TODO 4: Create the item's physics body
	// L08: TODO 3: Assign collider type
	// ...

	return true;
}

bool Item::Update(float dt)
{
	// L08: TODO 4: Sync the item's position from its physics body
	// ...

	return true;
}

bool Item::Draw()
{
	// L08: TODO 4: Once pbody exists, position is the body's CENTER -- offset
	// by half the sprite size when drawing
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX(), (int)position.getY());
	return true;
}

bool Item::CleanUp()
{
	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}

bool Item::Destroy()
{
	LOG("Destroying item");
	active = false;
	pendingToDelete = true;
	return true;
}
