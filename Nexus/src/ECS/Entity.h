#pragma once

#include <string>

class Coordinator;

class Entity
{
public:
	// Hold a pointer to the entity's owner coordinator
	Coordinator* coordinator{};

	explicit Entity(const size_t id) : m_id(id) {}
	Entity(const Entity& other) = default;

	[[nodiscard]] size_t GetId() const;
	void Kill() const;

	// For Entity to Entity comparisons
	Entity& operator= (const Entity& other) = default;
	bool operator== (const Entity& other) const { return m_id == other.GetId(); }
	bool operator!= (const Entity& other) const { return m_id != other.GetId(); }
	bool operator> (const Entity& other) const { return m_id > other.GetId(); }
	bool operator< (const Entity& other) const { return m_id < other.GetId(); }

	// Manage entity tags and groups
	void Tag(const std::string& tag) const;
	[[nodiscard]] bool HasTag(const std::string& tag) const;
	void Group(const std::string& group) const;
	[[nodiscard]] bool BelongsToGroup(const std::string& group) const;

	/// Manage direct Entity to Component interactions rather than via Coordinator
	template <typename TComponent, typename ...TArgs>
	void AddComponent(TArgs&& ...args);
	template <typename TComponent>
	void RemoveComponent() const;
	template <typename TComponent>
	[[nodiscard]] bool HasComponent() const;
	template <typename TComponent>
	TComponent& GetComponent() const;

private:
	size_t m_id;
};

template<typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args)
{
	coordinator->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent() const
{
	coordinator->RemoveComponent<TComponent>(*this);
}

template<typename TComponent>
bool Entity::HasComponent() const
{
	return coordinator->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const
{
	return coordinator->GetComponent<TComponent>(*this);
}
