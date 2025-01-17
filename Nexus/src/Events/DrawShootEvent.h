#pragma once

#include <optional>
#include <utility>

#include "src/ECS/Entity.h"
#include "src/EventManagement/IEvent.h"
#include "src/Physics/Contact.h"

/**
 * DragShootEvent
 * @param start (Vector2): Start mouse position
 * @param end (Vector2): End mouse position
*/
class DragShootEvent : public IEvent
{
public:
	Vector2 start;
	Vector2 end;

	DragShootEvent(const Vector2 start, const Vector2 end) :
		start(start), end(end)
	{}
};