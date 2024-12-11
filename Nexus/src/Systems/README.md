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
7. **RenderDebugSystem**: Supposed to render multiple useful info. Currently only renders red rectangles around the box Colliders.
8. **Animation System**: Alter animation component which contains data like isAnimating, animation frame. It also modifies the frame in sprite component which is like an idle frame. Render component then renders the active frame which is either a looping animation frame of idle frame in sprite component.

## Contains:

1. **Animation System**: Currently doesn't have a way to stop animation. Can either send an event from movement system once velocity is 0 but leaning towards a something like state system. Example for player the states could be Idle, Running, Jumping, Attack etc.
