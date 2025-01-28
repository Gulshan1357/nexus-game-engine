# GalaxyGolf

GalaxyGolf is a mini-golf game. It is responsible for
*. initializing all the systems (like `Trajectory System`, `Particle Effect System`), managers (like `Event Manager`, `Asset Manager`)
*. Spawning the world based on the selection (random wind speed and different gravity, atmosphere drag and ground color)
*. procedurally generate random terrain and obstacles.
*. spawn the player


## Contains files

1. **AbilitiesEnum**: Enum representing player abilities `NORMAL_SHOT`, `POWER_SHOT` and `WEAK_SHOT`.
2. **WorldSettings**: Contains `WorldType` Enum representing world type `EARTH`, `MARS` and `SUPER_EARTH and `WorldSettings` struct which contain world details like `gravity`, `Wind Speed` etc.
3. **GalaxyGolf**: The class representing the mini-golf game