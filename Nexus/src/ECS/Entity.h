#pragma once

class Coordinator;

class Entity
{
public:
	// Hold a pointer to the entity's owner coordinator
	Coordinator* m_Coordinator;

	Entity(int id) : m_id(id) {}

	[[nodiscard]] int GetId() const;
	void Kill() const;

	Entity& operator= (const Entity& other) = default;
	bool operator== (const Entity& other) const { return m_id == other.GetId(); }
	bool operator!= (const Entity& other) const { return m_id != other.GetId(); }
	bool operator> (const Entity& other) const { return m_id > other.GetId(); }
	bool operator< (const Entity& other) const { return m_id < other.GetId(); }

	/// Helper functions for direct Entity to Component interactions rather than through Coordinator
	template <typename TComponent, typename ...TArgs>
	void AddComponent(TArgs&& ...args);
	template <typename TComponent>
	void RemoveComponent() const;
	template <typename TComponent>
	bool HasComponent() const;
	template <typename TComponent>
	TComponent& GetComponent() const;

private:
	int m_id;
};

template<typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args)
{
	m_Coordinator->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent() const
{
	m_Coordinator->RemoveComponent<TComponent>(*this);
}

template<typename TComponent>
bool Entity::HasComponent() const
{
	return m_Coordinator->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
	return m_Coordinator->GetComponent<TComponent>(*this);
}
