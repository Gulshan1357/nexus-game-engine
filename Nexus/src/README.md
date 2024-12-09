# Src

## Contains

1. [**Utilities**](Utils/): Logger and Vector2
2. [**ECS**](ECS/): Contains Entity, Component and System classes along with Coordinator to manage everything.
3. [**Components**](Components/): Contain components like: Rigidbody, boxCollider, Sprite etc.
4. [**Systems**](Systems/): Contain systems like: Movement System, Render System, Input System etc.
5. [**Event Management**](EventManagement/): Blocking type event system. EventManager acts as an intermediate between pub-subs.
6. **Events**: Contain events like CollisionEvent, KeyPressEvent. They implement IEvent interface.
7. [**Input Management**](InputManagement/): Piggybacking on Event Management System, InputManager listens to keypress events and trigger the appropriate action.
8. [**AssetManagement**](AssetManagement/): It stores all the CSimpleSprite in a map<'tile-map', CSimpleSprite>, and provide helper function like AddSprite(), GetSprite so that the game can reuse it.
