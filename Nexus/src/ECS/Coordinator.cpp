#include "stdafx.h"
#include "Coordinator.h"

#include "Entity.h"

Entity Coordinator::CreateEntity()
{
	const Entity entity(0);
	Logger::Log("Entity created with id = " + std::to_string(0));

	return entity;
}
