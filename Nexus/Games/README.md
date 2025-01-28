# Games

Game is the the one that handle all the different GameStates {MENU, MAP_SELECT,	PLAYING, PAUSED, GAME_OVER}. It instantiated a new GalaxyGolf class when the GameState is set to playing and clears the pointer when `Gameover`.
For each map/level the Game instantiates a new GalaxyGolf class

## Contains folders

1. **UI**: Contain some helper functions to create UI effects like Concentric circles, color fade etc.
2. **GalaxyGolf**: The golf game. Since Game class need the overall score to display during the `GameOver` state, the GalaxyGolf propagates the accumulated score when it is closed.


## Contains files

1. **Game**: Responsible for the control between UI screens like game over, main menu and actual gameplay. It switches the screen based on the `GameState` Enum. When `Game::InitializeMap()` is called it create an instance of `GalaxyGolf` game.
2. **GameState**: Enum representing the state of the game. Possible values `MENU`, `PLAYING`, `PAUSED` etc.
3. **Score**: For scoring the total storks by each player. Currently only stores player one's score.