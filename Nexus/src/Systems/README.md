# Systems

Systems maintain a list of relevant entity IDs and, during each update(), iterate through them to perform calculations based on the values of the required components.

## Contains:

1. **Movement System**: Require Rigidbody and Transform component
2. **Render System**: Require Transform and Sprite Component.
3. **Collision System**: Require Transform and BoxCollider Component.
4. **Damage System**: Require BoxCollider Component. It doesn't update on every frame, rather it has an onCollision function which can be called by other system/events as callback. Currently it kill the collided entities.
5. **Render Text System**: It encapsulates the App::Print() function to render text.
6. **Input System**: The input system allows multiple players to control entities simultaneously. It consist of:
   - _InputEnum_: Where Enums for PlayerAction(like Jump, Move_up, Attack) and PlayerId(currently only has Player_1, Player_2) exist.
   - _KeyBindings_: This file stores all the keys and their corresponding action and provide some function to add/retrieve them.
   - _KeyPressEvent_: Contains playerId, action, player entity.
   - _InputSystem_: The system is subscribed to KeyPressEvent. When this event is fired it triggers the necessary action based on the event data.
7. **RenderDebugSystem**: Supposed to render multiple useful info. Currently renders outlines for various collider shapes (Box, sphere and polygon).
8. **Animation System**: Animate all the entities whose AnimationComponent's IsActive value is set to true. If bIsLooping is set to false, it only play the animation once. This value is supposed to be changed by other systems.
9. **Physics System**: This system calculates the inverse mass, angular mass and inverse angular mass of entities with RigidBody at the end of game::Initialize().

## TODO:
**Physics System**: Listening to collisions is very expensive. Use a different method to detect player's position.