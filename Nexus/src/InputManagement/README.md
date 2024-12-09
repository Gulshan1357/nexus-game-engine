# Input

The input system allows multiple players to control entities simultaneously. To add a new Input-Action:

- Update the InputEnums
- Add the keybinding in the initialize function using `m_keyBindings->AddKeyBinding(Input::PlayerID::PLAYER_1, 'W', Input::PlayerAction::MOVE_UP)`
- Inside the InputSystem, create a PerformActionfunction of type `void PerformMoveUpAction(Input::PlayerID playerId, Entity player)`
- Inside InputSystem::OnKeyPressed add an approprite if condition for the action and call the function like `if (event.action == Input::PlayerAction::MOVE_UP) { PerformMoveUpAction(event.playerId, event.player);}`

## Contains

1. **InputEnum**: Where Enums for PlayerAction(like Jump, Move_up, Attack) and PlayerId(currently only has Player_1, Player_2) exist.
2. **KeyBindings**: This file stores all the keys and their corresponding action and provide some function to add/retrieve them.
3. **KeyPressEvent**: Contains playerId, action, player entity.
4. **InputSystem**: The system is subscribed to KeyPressEvent. When this event is fired it triggers the necessary action based on the event data.

## TODO

Add persistance to key bindings. Use an external file to read and store data. This function should be added in KeyBindings.h file.
