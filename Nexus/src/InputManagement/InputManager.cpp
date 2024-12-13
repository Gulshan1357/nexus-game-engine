#include "stdafx.h"

#include "InputManager.h"

#include <iostream>

void InputManager::AddInputKeyToAction(const Input::PlayerID playerId, int key, const Input::PlayerAction action)
{
	if (playerId == Input::PlayerID::PLAYER_1)
	{

		m_playerOneKeyToActionMap.emplace(key, action);
		m_playerOneKeys.emplace_back(key);
		m_playerOneActionsStatus.emplace(action, false);
		//std::cout << "Key code = " + std::to_string(keyCode) + " Player Id= " + std::to_string(static_cast<int>(playerId)) + " Action= " + std::to_string(static_cast<int>(action));
	}
	else if (playerId == Input::PlayerID::PLAYER_2)
	{

		m_playerTwoKeyToActionMap.emplace(key, action);
		m_playerTwoKeys.emplace_back(key);
		m_playerTwoActionsStatus.emplace(action, true);
		//std::cout << "Key code = " + std::to_string(keyCode) + " Player Id= " + std::to_string(static_cast<int>(playerId)) + " Action= " + std::to_string(static_cast<int>(action));
	}
}

std::vector<InputManager::InputKey>& InputManager::GetAllKeys(const Input::PlayerID playerId)
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

Input::PlayerAction InputManager::GetActionEnum(const Input::PlayerID playerId, const int key) const
{
	const auto& keyToActionMap = (playerId == Input::PlayerID::PLAYER_1)
		? m_playerOneKeyToActionMap
		: m_playerTwoKeyToActionMap;
	if (const auto it = keyToActionMap.find(key); it != keyToActionMap.end())
	{
		return it->second;
	}
	// You shouldn't reach this stage
	throw std::runtime_error("InputManager::GetActionEnum() No action found!");
}

bool InputManager::GetActionStatus(Input::PlayerID playerId, Input::PlayerAction action)
{
	auto& actionMap = (playerId == Input::PlayerID::PLAYER_1)
		? m_playerOneActionsStatus
		: m_playerTwoActionsStatus;
	return actionMap[action];
}

void InputManager::UpdateActionStatus(Input::PlayerID playerId, Input::PlayerAction action, bool newStatus)
{
	auto& actionMap = (playerId == Input::PlayerID::PLAYER_1)
		? m_playerOneActionsStatus
		: m_playerTwoActionsStatus;
	if (auto it = actionMap.find(action); it != actionMap.end())
	{
		if (it->second != newStatus) it->second = newStatus;
	}
}

const std::unordered_map<Input::PlayerAction, bool>& InputManager::GetPlayerActionsStatus(Input::PlayerID playerId)
{
	return (playerId == Input::PlayerID::PLAYER_1)
		? m_playerOneActionsStatus
		: m_playerTwoActionsStatus;
}
