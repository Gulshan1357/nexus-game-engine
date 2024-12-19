# ECS: Entity Component System

## Contains:

1. **Entity**: Contains just an integer id and some helpful operator overload for Entity to Entity comparisons
2. **Component**: Contains an IComponent interface and ...
3. **Pool**: Contains an interface IPool and a template child class Pool<TComponent> which contains a vector of (contiguous data) of <TComponent>
4. **Coordinator**: Manages ECS
   - Entity Management: To add and delete entities at the start of Update()
   - Component Management: Template functions for each <TComponent>
   - System Management: Every system has a list of m_entities and a bitset m_componentSignature. Each System's Update() handles the modification to their respective <TComponent> which is called in game.Update(). Template functions for each <TSystem>

## Highlights:

1. Packed Entities: Reusing entity ids of deleted entity so that the are packed. This make sure that there is less data fragmentation in component pools.
2. Tags and Groups: Entities have groups and tags with average lookup speed of O(1) both ways (Entity <-> Tag/Group).
3. Packed Component Pools: When a component is deleted the last component of the vector will be moved to the empty position.
4. Entity-Entity Relationships: One to many relationship between entities. Create relationship using `leaderEntity->AddRelationship(followerEntity, "relationshipName")`.


## TODO:
1. Scenarios where all the entities of a particular tag are deleted. What should happen with that tag?