#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"

Scene::Scene() : Module()
{
	name = "scene";
	img = nullptr;
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

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	img = Engine::GetInstance().textures->Load("Assets/Textures/test.png");
	if (img == NULL) return false;

	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level-iv-339695.wav");

	// L06: TODO 7: Load the map through the map module
	// ...

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

	Engine::GetInstance().render->camera.x = (int)cameraX;
	Engine::GetInstance().render->camera.y = (int)cameraY;

	//Get the size of the window
	int windowW, windowH;
	Engine::GetInstance().window->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	int texW, texH;
	Engine::GetInstance().textures->GetSize(img, texW, texH);

	// Renders the image in the center of the screen
	Engine::GetInstance().render->DrawTexture(img, windowW /2 - texW / 2, windowH /2 - texH / 2);

	return true;
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

	SDL_DestroyTexture(img);

	return true;
}
