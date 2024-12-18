# Physics

2D physics implementation

- To add physics to any entity, it should have a RigidBody component with Boolean bUsePhysics sets to true. The default mass is set to 1.0f but can be changed accordingly. Adding collider is optional but strongly recommended
- To add a Force to an entity, use `rigidbody.AddForce(Vector2())`. Multiply the x and y values of force with `Physics::PIXEL_PER_METER` for proper scaling with respect to screen.

## Contains

1. **PhysicsEngine**: This engine contains helper function to add Forces and Integrate with respect to delta time. MovementSystem uses these functions to add stimulate Newtonian physics.
   - `GenerateDragForce()`: For drag due to different mediums like Air, water etc.
   - `GenerateFrictionForce()`: For frictional force.
   - `GenerateGravitationalForce()`: For Gravitational attraction between two bodies with mass. Need rigibody of the other entity to apply the opposite force.
   - `GenerateSpringForce()`: For Spring force between an anchor and a body. Need the transform component of the anchor.
2. **Constants**: Some constant value related to Physics.

## TODO

1. Currently using the Implicit Euler Integration technique. Try Verlet Integration or RK4.
2. Create helper functions for different forces so that they can be accessed via RigidBody.
