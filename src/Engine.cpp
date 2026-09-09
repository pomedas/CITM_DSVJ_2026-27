#include "Engine.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Log.h"

#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"


// Constructor
Engine::Engine() {

	LOG("Constructor Engine::Engine");

    // L02: TODO 3: Measure the amount of ms that takes to execute the Engine constructor and LOG the result
    // ...

    // Modules
    window = std::make_shared<Window>();
    input = std::make_shared<Input>();
    render = std::make_shared<Render>();
    textures = std::make_shared<Textures>();
    audio = std::make_shared<Audio>();
    scene = std::make_shared<Scene>();

    // Ordered for awake / Start / Update
    // Reverse order of CleanUp
    AddModule(std::static_pointer_cast<Module>(window));
    AddModule(std::static_pointer_cast<Module>(input));
    AddModule(std::static_pointer_cast<Module>(textures));
    AddModule(std::static_pointer_cast<Module>(audio));
    AddModule(std::static_pointer_cast<Module>(scene));

    // Render last
    AddModule(std::static_pointer_cast<Module>(render));

    // L02: TODO 3: Log the result of the timer
}

// Static method to get the instance of the Engine class, following the singleton pattern
Engine& Engine::GetInstance() {
    static Engine instance; // Guaranteed to be destroyed and instantiated on first use
    return instance;
}

void Engine::AddModule(std::shared_ptr<Module> module){
    module->Init();
    moduleList.push_back(module);
}

// Called before render is available
bool Engine::Awake() {

    // L02: TODO 3: Measure the amount of ms that takes to execute the Awake() and LOG the result
    // ...

    LOG("Engine::Awake");

    //Iterates the module list and calls Awake on each module
    bool result = true;
    for (const auto& module : moduleList) {
        result =  module->Awake();
        if (!result) {
			break;
		}
    }

    // L02: TODO 3: Log the result of the timer

    return result;
}

// Called before the first frame
bool Engine::Start() {

    // L02: TODO 3: Measure the amount of ms that takes to execute the Start() and LOG the result
    // ...

    LOG("Engine::Start");

    //Iterates the module list and calls Start on each module
    bool result = true;
    for (const auto& module : moduleList) {
        result = module->Start();
        if (!result) {
            break;
        }
    }

    // L02: TODO 3: Log the result of the timer

    return result;
}

// Called each loop iteration
bool Engine::Update() {

    bool ret = true;
    PrepareUpdate();

    if (input->GetWindowEvent(WE_QUIT) == true)
        ret = false;

    if (ret == true)
        ret = PreUpdate();

    if (ret == true)
        ret = DoUpdate();

    if (ret == true)
        ret = PostUpdate();

    FinishUpdate();
    return ret;
}

// Called before quitting
bool Engine::CleanUp() {

    // L02: TODO 3: Measure the amount of ms that takes to execute the CleanUp() and LOG the result
    // ...

    LOG("Engine::CleanUp");

    //Iterates the module list IN REVERSE and calls CleanUp on each module.
    //Modules are torn down in the opposite order to how they were created,
    //so a module never outlives something it depends on. Window::CleanUp()
    //calls SDL_Quit(), so it must run after Render has destroyed the renderer.
    bool result = true;
    for (auto it = moduleList.rbegin(); it != moduleList.rend(); ++it) {
        result = (*it)->CleanUp();
        if (!result) {
            break;
        }
    }

    // L02: TODO 3: Log the result of the timer

    return result;
}

// ---------------------------------------------
void Engine::PrepareUpdate()
{
    frameTime.Start();
}

// ---------------------------------------------
void Engine::FinishUpdate()
{
    // L02: TODO 4: Calculate:
    // Amount of frames since startup
    // Amount of time since game start (use a low resolution timer)
    // Amount of ms took the last update (dt)
    // Amount of frames during the last second
    // True lifetime average FPS: divide total frames by total elapsed time,
    // guarded against divide-by-zero on the very first frame, using the
    // millisecond-resolution Timer rather than the truncated integer
    // secondsSinceStartup
    // ...

    // Window title is expensive to rebuild and SetTitle() is a syscall, so it
    // is throttled to ~4 Hz; the measurements above are still per-frame.
    if (titleUpdateTime.ReadMs() > 250.0) {
        titleUpdateTime.Start();

        std::stringstream ss;
        ss << gameTitle << ": Av.FPS: " << std::fixed << std::setprecision(2) << averageFps
            << " Last sec frames: " << framesPerSecond
            << " Last dt: " << std::fixed << std::setprecision(3) << dt
            << " Time since startup: " << secondsSinceStartup
            << " Frame Count: " << frameCount;

        window->SetTitle(ss.str().c_str());
    }
}

// Call modules before each loop iteration
bool Engine::PreUpdate()
{
    //Iterates the module list and calls PreUpdate on each module
    bool result = true;
    for (const auto& module : moduleList) {
        result = module->PreUpdate();
        if (!result) {
            break;
        }
    }

    return result;
}

// Call modules on each loop iteration
bool Engine::DoUpdate()
{
    //Iterates the module list and calls Update on each module
    bool result = true;
    for (const auto& module : moduleList) {
        result = module->Update(dt);
        if (!result) {
            break;
        }
    }

    return result;
}

// Call modules after each loop iteration
bool Engine::PostUpdate()
{
    //Iterates the module list and calls PostUpdate on each module
    bool result = true;
    for (const auto& module : moduleList) {
        result = module->PostUpdate();
        if (!result) {
            break;
        }
    }

    return result;
}


