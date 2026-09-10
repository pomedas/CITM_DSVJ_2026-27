#pragma once

#include <memory>
#include <string>
#include "Vector2D.h"

enum class EntityType
{
	PLAYER,
	UNKNOWN
};

class Entity : public std::enable_shared_from_this<Entity>
{
public:

	Entity() {}
	Entity(EntityType type) : type(type), active(true) {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	// Logic only. EntityManager calls this from Update(), before any Draw().
	virtual bool Update(float dt)
	{
		return true;
	}

	// Rendering only. EntityManager calls this from PostUpdate(), once every
	// entity's Update() for the frame has already run.
	virtual bool Draw()
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Destroy()
	{
		return true;
	}

	void Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

public:

	std::string name;
	EntityType type = EntityType::UNKNOWN;
	bool active = true;
	bool pendingToDelete = false;

	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	Vector2D position;
	bool renderable = true;
};
