# Input System

The input system is **action-based** and supports multiple players controlling entities simultaneously.  

---

## Adding a New Input-Action:

1. **Update InputEnums**  
   - Define the new action in the `InputEnums`.

2. **Set Up Keybinding**  
   - Add the keybinding in the `initialize` function:  
     ```cpp
     m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, 'W', Input::PlayerAction::MOVE_UP);
     ```  
   - With this, the system will emit an `ActionChangeEvent` whenever the status of `MOVE_UP` changes.  
   - These events can be received by the `InputSystem` or other components.

3. **Handle Action in InputSystem**  
   - Create a function to handle the action, e.g.,  
     ```cpp
     void PerformMoveUp(Input::PlayerID playerId, Entity player);
     ```  
     - This function will respond to the `MOVE_UP` action using the entity's `InputComponentData`.  

4. **Update OnActionChange**  
   - Add a condition in `InputSystem::OnActionChange()` to trigger the appropriate action:  
     ```cpp
     if (event.action == Input::PlayerAction::MOVE_UP) { 
         PerformMoveUp(event.playerId, event.player); 
     }
     ```

---

## Contains:

1. **InputEnum**  
   - Stores enums for:  
     - `PlayerAction` (e.g., `Jump`, `Move_Up`, `Attack`).  
     - `PlayerID` (currently supports `PLAYER_1`, `PLAYER_2`).  

2. **InputManager**  
   - Manages key states and their corresponding actions.  
   - Provides functions to add/retrieve key-action mappings.

3. **ActionChangeEvent**  
   - Contains:  
     - `player entity`  
     - `playerId`  
     - `action`  
     - `action status`  

4. **InputSystem**  
   - Subscribed to `ActionChangeEvent`.  
   - Triggers the necessary action based on event data.  

---

## TODO:

- Add persistence to key bindings.  
  - Use an external file to read and store key-binding data.  
  - Implement this functionality in the `KeyBindings.h` file.
