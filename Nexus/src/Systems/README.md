# Systems
Systems maintain a list of relevant entity IDs and, during each update(), iterate through them to perform calculations based on the values of the required components.

## Contains:

1. **Movement System**: Require Rigidbody and Transform component
2. **Render System**: Require Transform and Sprite Component. Unfinished.
3. **Collision System**:  Require Transform and BoxCollider Component. Unfinished.
4. **Damage System**: Require BoxCollider Component. It doesn't update on every frame, rather it has an onCollision function which can be called by other system/events as callback. Currently it kill the colllided entities.
5. **Render Text System**: It encapsulates the App::Print() function to render text.

## TODO:
1. SpriteComponent should keep track of sprite asset location and 
2. RenderSystem should render based on the sprite location inside SpriteComponent
3. Collision System should emit events after collision. Need Event System