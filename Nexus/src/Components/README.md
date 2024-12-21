# Components
They store data. 

## Contains

1. **Transform Component**: Stores position, Scale and Rotation
2. **RigidBody Component**: Stores information like velocity, acceleration, mass and bUsePhysics. If bUsePhysics is false then we are directly integrating acceleration to increment velocity but if bUsePhysics is true then we are not using the acceleration at all. Rather we are using forces to change the acceleration.
3. **Sprite Component**: Should store location of sprite, zIndex (higher index entities will be rendered on top of lower indexed entities) and frame (frame to render when not animating).
4. **ColliderType Component**: A component which tells Collision system the shape of collider. Possible values are Box, Circle and Polygon. Always add this component along with the shape collider component.
   - **Box Collider Component**: Stores width, height and Offset for Box Collider
   - **Circle Collider Component**: Stores radius and Offset for Circle Collider
   - **Polygon Collider Component**: Stores lis of vertices and Offset for Polygon Collider
5. **UI Text Component**: A component to store UI text info like text string, position, color and font type.
6. **Animation Component**: A component to store info like:
   - *bIsActive**: Animation system will only play this if this is true
   - *animationLength*: Number of frames on the animation
   - *bIsLooping*: Is the animation looping, if not then the animation system will switch the bIsActive to false after the current frame reaches the end of animation
   - *animationFramesRow*: A single sprite usually contains multiple animation, this value represents which animation is currently active. 
   - *playFromBeginning*: If the animation was stopped before it reaches the end, then if it is re-activated, should the animation start from beginning or continue from where it was before.
7. **Input Component**: This component store information for player input. It store information like which player this is (Input::PlayerID playerId) and what should be the input values like upVelocity, rightVelocity, downVelocity and leftVelocity.

## Additional info regarding RigidBody
Beside RigidBody Component all the other components are struct. RigidBody is a class because it end up needing a bit more functionality. Like Unity3D game engine, I wanted the capability to add forces direclty from rigidbody like `rigidbody.AddForce(weight)`. So I made turned it to a class and made this a friend of PhysicsEngine class. And I created a wrapper to call `PhysicsEngine::AddForce(*this, force)` function Also, it made sense to store all the unapplied forces in rigidbody. But since it shouldn't be public, `m_sumForces` and  `m_inverseOfMass` (inverse of mass) are private variables and only PhysicsEngine can access them. 
