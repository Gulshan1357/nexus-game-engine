# ECS: Entity Component System

Contains:

1. **Entity**: Contains just an integer id and some helpful operator overload for Entity to Entity comparisons
2. **Component**: Contains an IComponent interface and ...
3. **Pool**: Contains an interface IPool and a template child class Pool<TComponent> which contains a vector of (contiguous data) of <TComponent>
4. **Coordinator**: Manages ECS
   - Entity Management: To add and delete entities at the start of Update()
   - Component Management: Template functions for each <TComponent>
   - System Management: ?

Highlights:

1. Packed Entities: Reusing entity ids of deleted entity so that the are packed. This make sure that there is less data fragmentation in component pools
