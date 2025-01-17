#pragma once

#include <memory>

#include "GameState.h"
#include "GalaxyGolf/MapType.h"
#include "src/Physics/Constants.h"
#include "src/Utils/Color.h"

enum class MapType;
struct Color;
class GalaxyGolf;

class Game
{
public:
	Game();
	~Game();

	void Initialize();
	void InitializeMap(MapType mapType);

	void Update(float deltaTime);
	void UpdateMenu(float deltaTime);
	void UpdateMapSelection(float deltaTime);
	void UpdateGame(float deltaTime);
	void UpdatePaused(float deltaTime);
	void UpdateGameOver(float deltaTime);

	void Render() const;
	void RenderMenu() const;
	void RenderMapSelection() const;
	void RenderGame() const;
	void RenderPaused() const;
	void RenderGameOver() const;

	// UI
	void RenderUILayout() const;
	void RenderUIControls() const;

	void Shutdown();
private:
	GameState m_currentState;
	std::unique_ptr<GalaxyGolf> game;

	// Map
	MapType m_selectedMap = MapType::EARTH;
	bool m_isMapInitialized = false;
	
	//------------------------------------------------------------------------
	// UI Styling
	//------------------------------------------------------------------------
	// Margin
	const float m_leftMargin = 100.0f;
	const float m_menuStartY = Physics::SCREEN_HEIGHT - 300.0f;
	const float m_menuSpacing = 40.0f;
	const float m_titleY = Physics::SCREEN_HEIGHT - 100.0f;
	const float m_controlsStartY = 250.0f;
	const float m_controlsSpacing = 20.0f;

	// Colors
	Color m_background = Color(0.2f, 0.08f, 0.1f); // Very Dark red
	Color m_accentPrimary = Color(1.0f, 0.35f, 0.3f); // red;
	Color m_accentSecondary = Color(Colors::NEON_BLUE); // red;
	Color m_fontTitle = Color(1.0f, 0.8f, 0.0f); // yellow
	Color m_fontPrimary = Color(1.0f, 0.35f, 0.3f); // red;
	Color m_fontSecondary = Color(0.31f, 0.81f, 0.87f); // blue;
};
