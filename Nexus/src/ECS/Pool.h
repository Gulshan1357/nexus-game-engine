#pragma once
#include <vector>

//------------------------------------------------------------------------
// IPool 
// A interface for pool which is just a vector (contiguous data) of objects of type Component<T>
//------------------------------------------------------------------------
class IPool
{
public:
	virtual ~IPool() = default;
};

//------------------------------------------------------------------------
// Pool 
// A pool is just a vector (contiguous data) of objects of type Component<T>
//------------------------------------------------------------------------
template <typename T>
class Pool : public IPool
{
public:
	Pool(int size = 100) { m_data.resize(size); }
	~Pool() override = default;

	[[nodiscard]] bool IsEmpty() const { return m_data.empty(); }
	[[nodiscard]] size_t GetSize() const { return m_data.size(); }
	void Resize(int n) { m_data.resize(n); }
	void Clear() { m_data.clear(); }
	void Add(T object) { m_data.push_back(object); }
	void Set(int index, T object) { m_data[index] = object; }
	T& Get(int index) { return static_cast<T&>(m_data[index]); }
	T& operator [](unsigned int index) { return m_data[index]; }

private:
	std::vector<T> m_data;
};