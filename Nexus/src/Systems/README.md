# Systems

Systems maintain a list of relevant entity IDs and, during each `update()`, iterate through them to perform calculations based on the values of the required components.

---

## Contains

1. **Render System**
   - Requires: `TransformComponent` and `SpriteComponent`.
   - Purpose: Responsible for rendering entities on the screen by updating their position and drawing their associated sprite.

2. **Collision System**
   - Requires: `TransformComponent` and `ColliderTypeComponent`.
   - Purpose: Detects collisions between entities using their colliders and triggers the appropriate reactions.

3. **Damage System**
   - Requires: `BoxColliderComponent`.
   - Purpose: Handles damage logic on collision.
     - It doesn't update every frame. Instead, it has an `onCollision` function that is triggered by other systems/events as a callback.
     - Currently, it kills the collided entities.

4. **Render Text System**
   - Requires: `UITextComponent`.
   - Purpose: Encapsulates the `App::Print()` function to render text on screen.

5. **Input System**
   - Requires: `InputComponent` and `AnimationComponent`.
   - The input system enables multiple players to control entities simultaneously. It consists of:
     - **`InputEnum`**: Contains Enums for `PlayerAction` (e.g., `Jump`, `Move_up`, `Attack`) and `PlayerId` (currently only `Player_1`, `Player_2`).
     - **`KeyBindings`**: Stores all key mappings and their corresponding actions, along with functions to add and retrieve key bindings.
     - **`KeyPressEvent`**: Contains `playerId`, `action`, and `player entity` data.
     - **`InputSystem`**: Subscribed to the `KeyPressEvent`. When the event is triggered, it executes the necessary actions based on the event data.

6. **Render Debug System**
   - Requires: `TransformComponent` and `ColliderTypeComponent`.
   - Purpose: Renders various debug information.
     - Currently renders outlines for different collider shapes (Box, Sphere, Polygon).

7. **Animation System**
   - Requires: `SpriteComponent` and `AnimationComponent`.
   - Purpose: Animates entities with an `AnimationComponent` that has `IsActive` set to `true`.
     - If `bIsLooping` is set to `false`, the animation is played only once.
     - The animation system controls these values based on other systems.

8. **Physics System**
   - Requires: `RigidBodyComponent` and `TransformComponent`.
   - Purpose:
     - Calculates inverse mass, angular mass, and inverse angular mass for entities with a `RigidBody` at the start of `game::Initialize()`.  
     - Handles movement logic by updating the entity's position based on its velocity and acceleration and also by resolving applied forces and torque.  
     - Listens to collision events and resolves collisions. 

---

## TODO

- **Physics System**:
  - Listening to collisions is very expensive. Use a different method to detect player's position.
