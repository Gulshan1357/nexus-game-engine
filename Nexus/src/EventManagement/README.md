# Event System

The event system implemented here is a **blocking type**, meaning the game is paused whenever an event is emitted until all listener callbacks are executed.

---

## Contains:

1. **IEvent**  
   - An interface for events.  
   - New events can be created by inheriting from this.

2. **EventCallback**  
   - A container for callback functions.  

3. **EventManager**  
   - Manages subscriptions and event dispatching.  
   - Includes an `m_subscribers` map, where:  
     - Keys: Event types.  
     - Values: Lists of associated callback functions.

---

## Limitation:

- **Current Behavior**  
  The `game.Update()` function clears and resubscribes all event callbacks repeatedly.  

- **Possible Optimization?**  
  Implement a buffer for subscriptions to add or remove callbacks only during specific "events" or for a given object ID.  
  Example: When an entity is removed, automatically delete all events associated with that entity.
