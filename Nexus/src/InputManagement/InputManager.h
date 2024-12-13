#pragma once

#include <vector>
#include <map>
#include <unordered_map>

#include "InputEnums.h"

//------------------------------------------------------------------------
// Contain a maps from Input to action and also a vector<int> for each player key codes for fast traversal.
//------------------------------------------------------------------------
class InputManager
{
public:
	struct InputKey
	{
		const int keyCode;
		bool bIsPressed;
		InputKey(int keyCode) : keyCode(keyCode), bIsPressed(false) {}
		InputKey(int keyCode, bool bIsPressed) : keyCode(keyCode), bIsPressed(bIsPressed) {}

		// Equality operator (required for unordered_map)
		bool operator==(const InputKey& other) const
		{
			return keyCode == other.keyCode;
		}
	};

	void AddInputKeyToAction(Input::PlayerID playerId, int key, Input::PlayerAction action);
	std::vector<InputKey>& GetAllKeys(Input::PlayerID playerId);
	Input::PlayerAction GetActionEnum(Input::PlayerID playerId, const int key) const;

	bool GetActionStatus(Input::PlayerID playerId, Input::PlayerAction action);
	void UpdateActionStatus(Input::PlayerID playerId, Input::PlayerAction action, bool newStatus);
	const std::unordered_map<Input::PlayerAction, bool>& GetPlayerActionsStatus(Input::PlayerID playerId);

	// TODO: Save and Retrieve Input to Action KeyBindings from external file.
	// void ParseJSONKeyBindings();

private:
	// Store all InputKeys(keycode, bIsPressed) for player 1. Used for comparing old and new bIsPressed
	std::vector<InputKey> m_playerOneKeys;
	// Store all InputKeys(keycode, bIsPressed) for player 2. Used for comparing old and new bIsPressed
	std::vector<InputKey> m_playerTwoKeys;

	// Key - Player Action map for player 1
	std::unordered_map<int, Input::PlayerAction> m_playerOneKeyToActionMap;
	// Key - Player Action map for player 2
	std::unordered_map<int, Input::PlayerAction> m_playerTwoKeyToActionMap;

	// List of actions and their state (Are they active or not?)
	std::unordered_map<Input::PlayerAction, bool> m_playerOneActionsStatus;
	std::unordered_map<Input::PlayerAction, bool> m_playerTwoActionsStatus;
};