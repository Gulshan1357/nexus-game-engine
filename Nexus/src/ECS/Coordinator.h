#pragma once

#include <src/Utils/Logger.h>

class Entity;

class Coordinator
{
public:
	Coordinator()
	{
		Logger::Log("Coordinator constructor called");
	}
	~Coordinator()
	{
		Logger::Log("Coordinator destructor called");
	}

	// Entity Management
	Entity CreateEntity();
};