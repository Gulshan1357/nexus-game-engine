# Components

Components store data and define the properties of entities in the game.

## Contains

1. **Transform Component**  
   - Stores position, scale, and rotation.

2. **RigidBody Component**  
   - Stores information such as velocity, acceleration, mass, and `bUsePhysics`.  
   - If `bUsePhysics` is `false`, acceleration is directly integrated to increment velocity.  
   - If `bUsePhysics` is `true`, forces are used to calculate acceleration.

3. **Sprite Component**  
   - Stores:  
     - Location of the sprite.  
     - `zIndex` (higher index entities are rendered on top of lower indexed entities).  
     - `frame` (the frame to render when not animating).

4. **ColliderType Component**  
   - Informs the collision system of the collider's shape. Possible values: `Box`, `Circle`, and `Polygon`.  
   - Always add this component along with a shape collider component.  
     - **Box Collider Component**: Stores width, height, and offset for box colliders. Constructor initialize the local vertices based on width and height.  
     - **Circle Collider Component**: Stores radius and offset for circle colliders. 
     - **Polygon Collider Component**: Stores a list of vertices and offset for polygon colliders.

5. **UI Text Component**  
   - Stores UI text information such as:  
     - Text string.  
     - Position.  
     - Color.  
     - Font type.

6. **Animation Component**  
   - Stores animation-related data:  
     - **`bIsActive`**: The animation system only plays this if `true`.  
     - **`animationLength`**: Number of frames in the animation.  
     - **`bIsLooping`**: Determines if the animation loops. If `false`, the animation system sets `bIsActive` to `false` when the animation ends.  
     - **`animationFramesRow`**: Indicates the active animation in a sprite containing multiple animations.  
     - **`playFromBeginning`**: If an animation is stopped before completion, this determines whether it starts from the beginning or resumes where it left off when reactivated.

7. **Input Component**  
   - Stores player input information, such as:  
     - Player identifier (`Input::PlayerID playerId`).  
     - Input values, e.g., `upVelocity`, `rightVelocity`, `downVelocity`, and `leftVelocity`.

---

## Additional Information Regarding RigidBody

Most components are implemented as structs, but the `RigidBody` component is implemented as a class due to its additional functionality.  

Like in Unity3D, the `RigidBody` class provides capabilities such as:  
- Adding forces, torque and impulse directly using methods like `rigidbody.AddForce(weight)`, `rigidBody.AddTorque()` and `rigidBody.ApplyImpulse()`.  
- Wrapping functionality to call `PhysicsEngine::AddForce(*this, force)` internally.  
- Storing unapplied forces and torque directly within the `RigidBody` class.  
