# Input

The input system is action based and allows multiple players to control entities simultaneously. To add a new Input-Action:

- Update the InputEnums
- Add the keybinding in the initialize function using `m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, 'W', Input::PlayerAction::MOVE_UP)`. With just this you should now be able to emit Action event when the status of Move_UP action is changed. These actions are received by InputSystem (but they can be received anywhere).
- Inside the InputSystem, create a PerformActionfunction of type `void PerformMoveUp(Input::PlayerID playerId, Entity player)`. Action event will contain the player entity, id, action and actionStatus. Entity can respond to these actions based on the InputComponentData.
- Inside InputSystem::OnActionChange(), add an approprite if condition for the action and call the function: `if (event.action == Input::PlayerAction::MOVE_UP) { PerformMoveUp(event.playerId, event.player);}`

## Contains

1. **InputEnum**: Where Enums for PlayerAction(like Jump, Move_up, Attack) and PlayerId(currently only has Player_1, Player_2) exist.
2. **InputManager**: This file stores all the keys, their state for comparison and their corresponding action. It also provide some function to add/retrieve them.
3. **ActionChangeEvent**: Contains player entity, playerId, action and action status.
4. **InputSystem**: The system is subscribed to OnActionChangeEvent. When this event is fired it triggers the necessary action based on the event data.

## TODO

Add persistence to key bindings. Use an external file to read and store data. This function should be added in KeyBindings.h file.
