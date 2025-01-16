# Physics System  

**2D Physics Implementation**  

---  

## Overview  

- **To enable physics for an entity:**  
  - Add a **RigidBody** component and set the boolean `bUsePhysics` to `true`.  
  - The default mass is set to `1.0f`, but it can be adjusted as needed.  
  - Adding a collider is optional but strongly recommended for accurate physics interactions. The Physics System calculates the Inverse Mass, Moment of Inertia (angular mass), and inverse angular mass using the `ColliderType` component (along with components like `BoxCollider`, `SphereCollider`, etc., for actual dimensions).  

- **To apply a force to an entity:**  
  - Use:  
    ```cpp  
    rigidbody.AddForce(Vector2());  
    ```  
  - Multiply the x and y components of the force by `Physics::PIXEL_PER_METER` to ensure proper scaling with respect to the screen.  

---  

## Contains  

1. **PhysicsEngine**  
   - The core of the physics system. Contains helper functions for applying forces and integrating motion with respect to delta time.  
   
   - **Functions for basic entity setup:**  
     - `InitializeEntityPhysics()`  
       - Computes the mass, inverse of mass, moment of inertia, and its inverse for entities.  
     - `CalculateMomentOfInertia()`  
       - Computes the moment of inertia for entities.  
       - Supports box and circle shapes and requires the `ColliderType` component.  
   
   - **Functions to integrate all the acting Forces and Torque:**  
     - `IntegrateForces()`  
       - Integrates forces and torque to change the linear and angular velocities of the `RigidBody` component.  
     - `IntegrateVelocities()`  
       - Integrates linear and angular velocities to change the position and rotation of the `Transform` component. Make sure to also update the collider using `UpdateColliderProperties()`.  
   
   - **Functions to update the properties of Collider:**  
     - `UpdateColliderProperties()`  
       - Updates properties like `globalCenter` (circle) and `globalVertices` (polygon) of the collider with respect to the `Transform` component.  

   - **Functions to simulate different Forces:**  
     - `GenerateDragForce()`  
       - Simulates drag forces due to mediums like air or water.  
     - `GenerateFrictionForce()`  
       - Simulates frictional forces.  
     - `GenerateGravitationalForce()`  
       - Calculates gravitational attraction between two bodies with mass.  
       - Applies opposite forces to each body and requires the other entity’s `RigidBody`.  
     - `GenerateSpringForce()`  
       - Calculates spring force:  
         - Between an anchor point and a body, or  
         - Between two entities.  

   - **Functions to Apply Forces and Torque:**  
     - `AddForce()`  
       - Adds to or subtracts from the rigidbody's `sumForces` vector.  
     - `ClearForce()`  
       - Sets `sumForces` to a zero vector.  
     - `AddTorque()`  
       - Adds to or subtracts from the rigidbody's `sumTorque` float.  
     - `ClearTorque()`  
       - Sets `sumTorque` to zero.  

   - **Functions to detect collisions:**  
     - `IsAABBCollision()`  
       - Checks if two boxes are colliding using Axis-Aligned Bounding Box.  
     - `IsSATCollision()`  
       - Checks if two convex polygons are colliding using the Separating Axis Theorem. It also updates the 'std::vector<Contact>& outContactPoints' for multi-contact collision resolution.   

   - **Functions to resolve collisions:** [Deprecated: Now the constraint system resolves the constraints after collision. This is more stable]  
     - `ResolvePenetration()`  
       - Adjusts the position so that the colliding bodies are just in contact. This is useful because if the bodies are moving very fast, they might intersect between frames.  
     - `ResolveCollision()`  
       - Executes the `ResolvePenetration()` method, then calculates the impulse and applies it to the two rigid bodies in opposite directions. Mass and the body's coefficient of restitution (elasticity) significantly affect the impulse.
  
2. **Constants**  
   - Stores constants related to physics calculations.  

3. **Contact**  
   - The contact information when two entities collide. The `Collision Event` pass this information to the concerned systems. 

4. **PenetrationConstraint**  
   - Stores the penetration constrain information between two colliding entities. `ConstraintSystem` manages a vector of penetration constraints that are added during collisions and cleared after resolution 

5. **Particle**  
   - The particle information used by the `ParticleEffect System` and `ParticleEmitter Component`.    

6. **Camera**  
   - An orthographic camera with useful function like `SetPosition()`, `Move()`, `GetPosition()`. The Camera should be passed to all the systems related to game rendering.      

---  

## TODO  

1. **Improve Integration Technique**  
   - Currently using **Implicit Euler Integration**. Switch to:  
     - **Verlet Integration** (leaning towards this), or  
     - **RK4 (Runge-Kutta 4th Order)** for better accuracy.  

2. **Force Helper Functions**  
   - Add helper functions to the `RigidBody` class for easier access to different force generation methods.  
