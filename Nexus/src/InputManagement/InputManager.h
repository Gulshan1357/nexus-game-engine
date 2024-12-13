#pragma once

#include <vector>
#include <map>

#include "InputEnums.h"

//------------------------------------------------------------------------
// Contain a maps from Input to action and also a vector<int> for each player key codes for fast traversal.
//------------------------------------------------------------------------
class InputManager
{
public:
	void AddInputKeyToAction(Input::PlayerID playerId, int keyCode, Input::PlayerAction action);
	const std::vector<int>& GetAllKeys(Input::PlayerID playerId);
	Input::PlayerAction GetAction(Input::PlayerID playerId, int key);

	// TODO: Save and Retrieve Input to Action KeyBindings from external file.
	// void ParseJSONKeyBindings();

private:
	// Store all keycodes for each individual player so at to iterate over them every frame
	std::vector<int> m_playerOneKeys;
	std::vector<int> m_playerTwoKeys;

	// Key - Player Action map for player 1
	std::map<int, Input::PlayerAction> m_playerOneKeyToActionMap;
	// Key - Player Action map for player 2
	std::map<int, Input::PlayerAction> m_playerTwoKeyToActionMap;

};