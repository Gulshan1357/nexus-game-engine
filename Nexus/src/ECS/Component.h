#pragma once

struct IComponent
{
protected:
	static int m_nextId;
};

// Allocating memory for the static variable
int IComponent::m_nextId = 0;

template <typename T>
class Component : public IComponent
{
public:
	/* The first time GetId() is called for a Component<T>, it'll create a unique id which will remain same for each subsequent calls since id is also static */
	static int GetId()
	{
		static auto id = m_nextId++;
		return id;
	}
};