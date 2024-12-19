#pragma once

#include <string>
#include <vector>

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

	// Helper functions to manage Entity to Entity relationship via coordinator
	void AddRelationship(Entity target, const std::string& relationshipTag) const; // Add a relationship with the target entity
	void RemoveRelationship(Entity target, const std::string& relationshipTag) const; // Remove a relationship with target entity
	[[nodiscard]] std::vector<std::pair<Entity, std::string>> GetRelationships() const; // Returns a vector<Entity, relationshipTag> containing all the related entities
	[[nodiscard]] bool HasRelationship(Entity target, const std::string& relationshipTag) const; // Check if there exist a relationship with target entity
	[[nodiscard]] std::vector<Entity> GetEntitiesByRelationshipTag(const std::string& relationshipTag) const; // Return a vector of related entities that match relationship tag

	// Helper functions to manage Entity to Component interactions via coordinator
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

// Specialize std::hash for Entity so that we can use Entity as a key for unordered_multimap for entity-entity relationship
namespace std
{
	template<>
	struct hash<Entity>
	{
		std::size_t operator()(const Entity& entity) const noexcept
		{
			return std::hash<size_t>()(entity.GetId());
		}
	};
}
