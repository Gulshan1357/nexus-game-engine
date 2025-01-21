# Src

## Contains

1. [**Utilities**](Utils/)  
   - Includes `Logger` and `Vector2`.

2. [**ECS**](ECS/)  
   - Contains `Entity`, `Component`, and `System` classes, along with the `Coordinator` to manage everything.

3. [**Components**](Components/)  
   - Contains components such as `RigidBody`, `BoxCollider`, `Sprite`, etc.

4. [**Systems**](Systems/)  
   - Includes systems such as `MovementSystem`, `RenderSystem`, `InputSystem`, etc.

5. [**Event Management**](EventManagement/)  
   - Implements a blocking-type event system. `EventManager` acts as an intermediary between publishers and subscribers.

6. [**Events**](Events/)    
   - Contains events like `CollisionEvent` and `KeyPressEvent`. These implement the `IEvent` interface.

7. [**Input Management**](InputManagement/)  
   - Built on top of the Event Management System. `InputManager` listens to keypress events and triggers appropriate actions.

8. [**Asset Management**](AssetManagement/)  
   - Stores all `CSimpleSprite` objects in a map `<tile-map, CSimpleSprite>`. Provides helper functions like `AddSprite()` and `GetSprite()` to enable reuse of sprites in the game.

9. [**Audio Management**](AssetManagement/)  
   - Contains `AudioAsset` class that encapsulates the file path of the audio asset along with `AudioManager` to manage everything.

10. [**Physics**](Physics/)  
   - Contains the Physics Engine, which provides functions to generate various forces and torques and integrate them into position and rotation.

11. [**Procedural Content**](PCG/)  
   - Contains code to generate procedural terrain points and obstacles based on the terrain.
