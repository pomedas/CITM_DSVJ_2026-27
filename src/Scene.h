#pragma once

#include "Module.h"
#include <memory>

class Player;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// The L03 float camera position -- the only place that casts to int and
	// writes render->camera, so nothing else (e.g. Player's camera follow)
	// ever has to touch render->camera.x/y directly.
	void SetCameraX(float x);
	void SetCameraY(float y);

private:
	// Camera position kept as float so movement scaled by dt isn't lost to
	// integer truncation; converted to int only when written into
	// render->camera.
	float cameraX = 0.0f;
	float cameraY = 0.0f;

	std::shared_ptr<Player> player;
};