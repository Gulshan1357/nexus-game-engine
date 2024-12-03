#pragma once

#include <vector>
#include <unordered_map>

//------------------------------------------------------------------------
// IPool 
// A interface for pool which is just a vector (contiguous data) of objects of type Component<T>
//------------------------------------------------------------------------
class IPool
{
public:
	virtual ~IPool() = default;
	virtual void RemoveEntityFromPool(int entityId) = 0;
};

//------------------------------------------------------------------------
// Pool 
// A pool is just a vector (contiguous data) of objects of type Component<T>
//------------------------------------------------------------------------
template <typename T>
class Pool : public IPool
{
public:
	Pool(int capacity = 100) { m_data.reserve(capacity); }
	~Pool() override = default;

	[[nodiscard]] bool IsEmpty() const { return m_data.empty(); }
	[[nodiscard]] int GetSize() const { return m_data.size(); }
	void Resize(int n)
	{
		//m_data.resize(n);
		// reserve() should be faster compared to resize(). Because it only allocates the memory and doesn't initialize it. It also doesn't change size() so we can keep using it instead of declaring a separate size variable.
		m_data.reserve(n);
	}
	void Clear() { m_data.clear(); }
	//void Add(T object) { m_data.push_back(object); } // Use Set to add objects
	void Set(int entityId, T object)
	{
		if (m_entityToIndex.find(entityId) != m_entityToIndex.end())
		{
			// If the element already exist, replace the component object
			int index = m_entityToIndex[entityId];
			m_data[index] = object;
		}
		else
		{
			int index = m_data.size();
			m_entityToIndex.emplace(entityId, index);
			m_indexToEntity.emplace(index, entityId);
			if (index >= m_data.capacity())
			{
				m_data.reserve(m_data.capacity() * 2);
			}
			m_data.push_back(object);
		}
	}

	void Remove(int entityId)
	{
		// Copy the last element to the deleted position to keep the array packed
		int indexOfRemoved = m_entityToIndex[entityId];
		int indexOfLast = m_data.size() - 1;
		m_data[indexOfRemoved] = m_data[indexOfLast];

		// Update the index-entity maps to point to the correct elements
		int entityIdOfLastElement = m_indexToEntity[indexOfLast];
		m_entityToIndex[entityIdOfLastElement] = indexOfRemoved;
		m_indexToEntity[indexOfRemoved] = entityIdOfLastElement;

		// Remove the last element from the vector
		m_data.pop_back();

		// Remove the mappings for the removed entity
		m_entityToIndex.erase(entityId);
		m_indexToEntity.erase(indexOfLast);
	}

	void RemoveEntityFromPool(int entityId)  override
	{
		if (m_entityToIndex.find(entityId) != m_entityToIndex.end())
		{
			Remove(entityId);
		}
	}

	T& Get(const int entityId)
	{

		int index = m_entityToIndex[entityId];
		return static_cast<T&>(m_data[index]);
	}
	T& operator [](unsigned int index) { return m_data[index]; }

private:
	// The packed dynamic array of components (of generic type T). Didn't use the static array because it's size can't be changed. But later might switch to that based on the game for performance gains.
	std::vector<T> m_data;

	// TODO: Currently using unordered_maps to map between entity id and component pool index but it does have a performance penalty since unordered_maps are not contiguous. I used this because it has handy functions like find(), insert() and delete(). Switch to array.

	// Map from an entity ID to a component pool index.
	std::unordered_map<int, int> m_entityToIndex;

	// Map from a component pool index to an entity ID.
	std::unordered_map<int, int> m_indexToEntity;
};