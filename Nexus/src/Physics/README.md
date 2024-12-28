# Physics System  

**2D Physics Implementation**  

---

## Overview  

- To enable physics for an entity:  
  - Add a **RigidBody** component and set the boolean `bUsePhysics` to `true`.  
  - The default mass is set to `1.0f`, but it can be adjusted as needed.  
  - Adding a collider is optional but strongly recommended for accurate physics interactions. The Physics System calculate the Inverse Mass, Moment of Inertia (angular mass) and inverse angular mass using ColliderType component (along with components like BoxCollider, SphereCollider etc. for actual dimensions)  

- To apply a force to an entity:  
  - Use:  
    ```cpp
    rigidbody.AddForce(Vector2());
    ```  
  - Multiply the x and y components of the force by `Physics::PIXEL_PER_METER` to ensure proper scaling with respect to the screen.  

---

## Contains  

1. **PhysicsEngine**  
   - The core of the physics system. Contains helper functions for applying forces and integrating motion with respect to delta time.  
   - **Functions related to integrating force and torque:**  
     - `IntegrateLinear()`  
       - Calculates a new position by integrating the RigidBody's velocity and acceleration.  
     - `IntegrateAngular()`  
       - Calculates a new rotation by integrating the RigidBody's angular velocity and angular acceleration.  
     - `CalculateMomentOfInertia()`  
       - Computes the moment of inertia for entities.  
       - Supports box and circle shapes and requires the `ColliderType` component.
   - **Functions to simulate different Forces:**  
     - `GenerateDragForce()`  
       - Simulates drag forces due to mediums like air or water.  
     - `GenerateFrictionForce()`  
       - Simulates frictional forces.  
     - `GenerateGravitationalForce()`  
       - Calculates gravitational attraction between two bodies with mass.  
       - Applies opposite forces to each body and requires the other entity’s RigidBody.  
     - `GenerateSpringForce()`  
       - Calculates spring force:  
         - Between an anchor point and a body, or  
         - Between two entities.
     - `GenerateGravitationalForce()`  
       - Calculates gravitational attraction between two bodies with mass.
   - **Functions to detect collisions:**  
     - `IsAABBCollision()`  
       - Check if two boxes are colliding using Axis-Aligned Bounding Box.
     - `IsSATCollision()`  
       - Check if two convex polygons are colliding using the Separating Axis Theorem.
   - **Functions to resolve collisions:**  
     - `ResolvePenetration()`  
       - The projection method adjusts the position so that the colliding bodies are just in contact. This is useful because if the bodies are moving very fast, it’s possible that they touch between frames, and in the current frame, one body might be inside another.  
     - `ResolveCollision()`  
       - Executes the `ResolvePenetration()` method, then calculates the impulse and applies it to the two rigid bodies in opposite directions. Mass and the body's coefficient of restitution (elasticity) significantly affect the impulse.  

2. **Constants**  
   - Stores constants related to physics calculations.  

---

## TODO  

1. **Improve Integration Technique**  
   - Currently using **Implicit Euler Integration**. Switch to:  
     - **Verlet Integration** or (Leaning towards this)  
     - **RK4 (Runge-Kutta 4th Order)** for better accuracy.  

2. **Force Helper Functions**  
   - Add helper functions to the `RigidBody` class for easier access to different force generation methods.  
