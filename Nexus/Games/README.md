# Games

Game is the the one that handle all the different GameStates {MENU, MAP_SELECT,	PLAYING, PAUSED, GAME_OVER}. It instantiated a new GalaxyGolf class when the GameState is set to playing and clears the pointer when `gameover`.
For each map/level the Game instantiates a new GalaxyGolf class

## Contains folders

1. **UI**: Contain some helper fucnctions to create UI effects like Concentric circles, color fade etc.
2. **GalaxyGolf**: The golf game. Since Game class need the overall score to display during the GameOver state, the galaxygolf propagates the accumulated score when it is closed.