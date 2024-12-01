#pragma once

#include "../Components/TransformComponent.h"
#include "../ECS/Entity.h"
#include "../ECS/System.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
	}

	void Update(double deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			// Logger::Log(
			//     "Entity id = " + 
			//     std::to_string(entity.GetId()) + 
			//     " position is now (" + std::to_string(transform.position.x) + 
			//     ", " + std::to_string(transform.position.y) + 
			//     ")");
			Logger::Log(
				"Entity id = " +
				std::to_string(entity.GetId()) +
				" position is now changed because Movement System said so!!!");
		}
	}

};