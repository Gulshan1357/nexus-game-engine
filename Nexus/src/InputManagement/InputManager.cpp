#include "stdafx.h"

#include "InputManager.h"

#include <iostream>

void InputManager::AddKeyBinding(const Input::PlayerID playerId, const int keyCode, const Input::PlayerAction action)
{
	if (playerId == Input::PlayerID::PLAYER_1)
	{

		m_playerOneKeyBindings[keyCode] = action;
		m_playerOneKeys.push_back(keyCode);
		//std::cout << "Key code = " + std::to_string(keyCode) + " Player Id= " + std::to_string(static_cast<int>(playerId)) + " Action= " + std::to_string(static_cast<int>(action));
	}
	else if (playerId == Input::PlayerID::PLAYER_2)
	{

		m_playerTwoKeyBindings[keyCode] = action;
		m_playerTwoKeys.push_back(keyCode);
		//std::cout << "Key code = " + std::to_string(keyCode) + " Player Id= " + std::to_string(static_cast<int>(playerId)) + " Action= " + std::to_string(static_cast<int>(action));
	}
}

const std::vector<int>& InputManager::GetAllKeys(const Input::PlayerID playerId)
{
	switch (playerId)
	{
		case Input::PlayerID::PLAYER_1:
			return m_playerOneKeys;
		case Input::PlayerID::PLAYER_2:
			return m_playerTwoKeys;
		default:
			throw std::invalid_argument("Invalid PlayerID");
	}
}

Input::PlayerAction InputManager::GetAction(const Input::PlayerID playerId, const int key)
{
	const auto& keyBindings = (playerId == Input::PlayerID::PLAYER_1)
		? m_playerOneKeyBindings
		: m_playerTwoKeyBindings;
	if (const auto it = keyBindings.find(key); it != keyBindings.end())
	{
		return it->second;
	}
	// You shouldn't reach this stage
	throw std::runtime_error("InputManager::GetAction() No action found!");
}
