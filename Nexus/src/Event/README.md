# Event
The event system that I implemented here is a blocking type. This means that as soon as something emits an event the game will be blocked until all the listener callbacks are executed first.

## Contains

1. **IEvent**: An interface for event. New events can be created by inheriting from this.
2. **EventCallback**: A container for callback functions.
3. **EventBus**: Contains an m_subscribers map. This map will contain event as key and a list of callback functions as Values

## Limitation
Currently the game's update() function clears and resubscribe all event callback over and over. Possiblity of optimization: A buffer of subscriptions that are only added and removed at certain "events" or for a certain object ID. Example, when an entity is removed, remove all the events associated with that entity.