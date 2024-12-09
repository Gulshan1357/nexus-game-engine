# Systems

Systems maintain a list of relevant entity IDs and, during each update(), iterate through them to perform calculations based on the values of the required components.

## Contains:

1. **Movement System**: Require Rigidbody and Transform component
2. **Render System**: Require Transform and Sprite Component. Unfinished.
3. **Collision System**: Require Transform and BoxCollider Component. Unfinished.
4. **Damage System**: Require BoxCollider Component. It doesn't update on every frame, rather it has an onCollision function which can be called by other system/events as callback. Currently it kill the collided entities.
5. **Render Text System**: It encapsulates the App::Print() function to render text.
6. **Input System**: The input system allows multiple players to control entities simultaneously. It consist of:
   - _InputEnum_: Where Enums for PlayerAction(like Jump, Move_up, Attack) and PlayerId(currently only has Player_1, Player_2) exist.
   - _KeyBindings_: This file stores all the keys and their corresponding action and provide some function to add/retrieve them.
   - _KeyPressEvent_: Contains playerId, action, player entity.
   - _InputSystem_: The system is subscribed to KeyPressEvent. When this event is fired it triggers the necessary action based on the event data.

## TODO:

1. SpriteComponent should keep track of sprite asset location and
2. RenderSystem should render based on the sprite location inside SpriteComponent
3. Collision System should emit events after collision. Need Event System
