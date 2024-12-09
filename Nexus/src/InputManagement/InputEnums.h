#pragma once

namespace Input
{
	enum class PlayerAction
	{
		JUMP,
		MOVE_UP,
		MOVE_RIGHT,
		MOVE_DOWN,
		MOVE_LEFT,
		ATTACK
	};

	enum class PlayerID
	{
		PLAYER_1,
		PLAYER_2
	};

	// TODO: read from an external fine and populate the maps for keybindings for each player
	// For Player 1

	//enum class Device
	//{
	//	KEYBOARD,
	//	MOUSE,
	//	JOYSTICK_1
	//};

	//enum struct BindingType
	//{
	//	KEY,
	//	AXIS2D
	//};

	//struct Binding
	//{
	//	Device device;
	//	BindingType type;
	//	// This is std::variant<int, std::pair<float, float>>: `int` for keys, `std::pair<float, float>` for 2D axis values
	//	int value;

	//	/*Binding(Device device, BindingType type, std::variant <int, std::pair<float, float>> value)
	//		: device(device), type(type), value(value)
	//	{}*/

	//	Binding(Device device, BindingType type, int value)
	//		: device(device), type(type), value(value)
	//	{}

	//	bool operator==(const Binding& binding) const;
	//	bool operator!=(const Binding& binding) const;
	//};

	//inline bool Binding::operator==(const Binding& binding) const
	//{
	//	return device == binding.device && type == binding.type && value == binding.value;
	//}

	//inline bool Binding::operator!=(const Binding& binding) const
	//{
	//	return device != binding.device || type != binding.type || value != binding.value;
	//}
}



