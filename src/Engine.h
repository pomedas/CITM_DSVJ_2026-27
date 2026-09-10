#pragma once

#include <memory>
#include <list>
#include <string>
#include "Module.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "pugixml.hpp"


// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class Map;
class Physics;
class EntityManager;

class Engine
{
public:

	// Public method to get the instance of the Singleton
	static Engine& GetInstance();

	//
	void AddModule(std::shared_ptr<Module> module);

	// Delta time in seconds for the current frame, for modules -- e.g.
	// Physics -- that need it outside DoUpdate()'s Update(dt) call.
	float GetDt() const { return dt; }

	// A named child of <config> -- e.g. "player" -- for callers that aren't
	// a Module and so don't get their slice handed to them automatically
	// via LoadParameters() (Entities: see Entity::SetParameters()).
	pugi::xml_node GetConfigNode(const char* name) const { return configFile.child("config").child(name); }

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

private:

	// Private constructor to prevent instantiation
	// Constructor
	Engine();

	// Delete copy constructor and assignment operator to prevent copying
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load config file
	bool LoadConfig();

	std::list<std::shared_ptr<Module>> moduleList;

public:

	enum EngineState
	{
		CREATE = 1,
		AWAKE,
		START,
		LOOP,
		CLEAN,
		FAIL,
		EXIT
	};

	// Modules
	std::shared_ptr<Window> window;
	std::shared_ptr<Input> input;
	std::shared_ptr<Render> render;
	std::shared_ptr<Textures> textures;
	std::shared_ptr<Audio> audio;
	std::shared_ptr<Scene> scene;
	std::shared_ptr<Map> map;
	std::shared_ptr<Physics> physics;
	std::shared_ptr<EntityManager> entityManager;

private:
	// Delta time in seconds, computed once per frame in FinishUpdate().
	float dt = 0.0f;

	// Frames since startup.
	int frameCount = 0;

	// Low-resolution timer for the whole game life; PerfTimer wherever
	// sub-millisecond precision matters.
	Timer startupTime;
	PerfTimer frameTime;
	PerfTimer lastSecFrameTime;
	PerfTimer titleUpdateTime;

	int framesPerSecond = 0;
	int lastSecFrameCount = 0;

	float averageFps = 0.0f;
	int secondsSinceStartup = 0;

	// Maximum frame duration in milliseconds.
	int maxFrameDuration = 16;

	// Fractional millisecond left over from the last SDL_Delay() truncation,
	// carried into the next frame so the cap converges on maxFrameDuration
	// instead of running systematically fast.
	double delayRemainder = 0.0;

	std::string gameTitle = "Platformer Game";

	// L05: TODO 2: Parsed contents of config.xml
	pugi::xml_document configFile;
};
