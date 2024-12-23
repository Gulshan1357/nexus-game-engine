# ECS: Entity Component System

## Contains:

1. **Entity**  
   - Stores a unique integer ID.  
   - Includes helpful operator overloads for `Entity` to `Entity` comparisons.

2. **Component**  
   - Contains an `IComponent` interface.

3. **Pool**  
   - Includes an interface `IPool` and a templated child class `Pool<TComponent>`.  
   - `Pool<TComponent>` stores a contiguous vector of `<TComponent>` data.

4. **Coordinator**  
   - Manages the ECS, including:  
     - **Entity Management**: Handles adding and deleting entities at the start of `Update()`.  
     - **Component Management**: Provides templated functions for managing `<TComponent>`.  
     - **System Management**:  
       - Each system maintains a list of `m_entities` and a bitset `m_componentSignature`.  
       - The system's `Update()` method modifies its respective `<TComponent>` and is called in `game.Update()`.  
       - Includes templated functions for managing `<TSystem>`.

---

## Highlights:

1. **Packed Entities**  
   - Reuses IDs of deleted entities to keep them packed.  
   - Reduces data fragmentation in component pools.

2. **Tags and Groups**  
   - Entities can belong to tags and groups.  
   - Average lookup speed: **O(1)** (both ways: `Entity <-> Tag/Group`).

3. **Packed Component Pools**  
   - When a component is deleted, the last component in the vector is moved to the empty position to maintain packing.

4. **Entity-Entity Relationships**  
   - Supports one-to-many relationships between entities.  
   - Create relationships using:  
     ```cpp
     leaderEntity->AddRelationship(followerEntity, "relationshipName");
     ```

---

## TODO:

1. Scenarios where all the entities of a particular tag are deleted. What should happen with that tag?
