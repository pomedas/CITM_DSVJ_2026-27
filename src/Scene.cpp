#include "Engine.h"
#include "Input.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "EntityManager.h"
#include "Player.h"
#include "Item.h"
#include "Map.h"

Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	// L04: TODO 5: Create the player through the entity manager
	player = std::dynamic_pointer_cast<Player>(Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER));

	// L10: TODO 2: Hand the player its <player> config.xml section -- works
	// because the L04 pending queue defers Awake()/Start() until later, so
	// there's a window between construction and Awake() to set this first
	// ...

	// L08: TODO 5: Create an item through the entity manager, to test picking it up
	std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM));
	item->position = Vector2D(200, 672);

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");

	// L06: TODO 7: Load the map through the map module
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	// L03: TODO 3: Make the camera movement independent of framerate.
	// Move cameraX/cameraY by camSpeed * dt (pixels per second), then write
	// the rounded result into render->camera.x / render->camera.y.
	float camSpeed = 200.0f;

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		cameraY -= camSpeed * dt;

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		cameraY += camSpeed * dt;

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		cameraX -= camSpeed * dt;

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		cameraX += camSpeed * dt;

	SetCameraX(cameraX);
	SetCameraY(cameraY);

	return true;
}

void Scene::SetCameraX(float x)
{
	cameraX = x;
	Engine::GetInstance().render->camera.x = (int)cameraX;
}

void Scene::SetCameraY(float y)
{
	cameraY = y;
	Engine::GetInstance().render->camera.y = (int)cameraY;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
