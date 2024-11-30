#pragma once

class Coordinator;

class Entity
{
public:
	// Hold a pointer to the entity's owner coordinator
	Coordinator* mCoordinator;

	Entity(int id) : m_id(id) {}

	[[nodiscard]] int GetId() const;

	Entity& operator= (const Entity& other) = default;
	bool operator== (const Entity& other) const { return m_id == other.GetId(); }
	bool operator!= (const Entity& other) const { return m_id != other.GetId(); }
	bool operator> (const Entity& other) const { return m_id > other.GetId(); }
	bool operator< (const Entity& other) const { return m_id < other.GetId(); }

private:
	int m_id;
};

inline int Entity::GetId() const { return m_id; }