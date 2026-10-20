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
	Engine::GetInstance().textures->GetSize(texture, texW, texH);

	// L08: TODO 4: Add a physics to an item - initialize the physics body

	// L08: TODO 7: Assign collider type

	return true;
}

bool Item::Update(float dt)
{
	// L08: TODO 4: Add a physics to an item - update the position of the object from the physics.
	// ...

	return true;
}

bool Item::Draw()
{
	// L08: TODO 4: Add a physics to an item - update the position of the object from the physics.
	Engine::GetInstance().render->DrawTexture(texture, (int)position.getX(), (int)position.getY());
	return true;
}

bool Item::CleanUp()
{
	// L08: TODO 4: Release the physics body here before unloading the texture
	Engine::GetInstance().textures->UnLoad(texture);
	return true;
}

bool Item::Destroy()
{
	LOG("Destroying item");
	pendingToDelete = true;
	return true;
}
