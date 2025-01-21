#include "stdafx.h"
#include "Game.h"

#include "App/app.h"

#include "GalaxyGolf/GalaxyGolf.h"

#include "src/Physics/Constants.h"
#include "src/Utils/GraphicsUtils.h"
#include "src/Utils/Logger.h"
#include "src/Utils/Vector2.h"
#include "UI/UIEffects.h"

Game::Game()
{
	m_currentGameState = std::make_shared<GameState>();
	m_score = std::make_shared<Score>();
}

Game::~Game()
{
	m_currentGameState.reset();
	m_score.reset();
};

void Game::Initialize() const
{
	// For debug
	// *m_currentGameState = GameState::PLAYING;
}

void Game::InitializeMap(WorldType worldType, std::weak_ptr<GameState> gameState, std::weak_ptr<Score> score)
{
	m_game = std::make_unique<GalaxyGolf>(worldType, std::move(gameState), std::move(score));
	m_game->Initialize();
	m_isWorldInitialized = true;
}

void Game::Update(const float deltaTime)
{
	switch (*m_currentGameState)
	{
		case GameState::MENU:
			UpdateMenu(deltaTime);
			break;
		case GameState::MAP_SELECT:
			UpdateMapSelection(deltaTime);
			break;
		case GameState::PLAYING:
			UpdateGame(deltaTime);
			break;
		case GameState::PAUSED:
			UpdatePaused(deltaTime);
			break;
		case GameState::GAME_OVER:
			UpdateGameOver(deltaTime);
			break;
	}
}

void Game::UpdateMenu(float deltaTime) const
{
	// Enter to play
	if (App::IsKeyPressed(VK_RETURN))
	{
		*m_currentGameState = GameState::MAP_SELECT;
	}

	// App/main.h watch the APP_QUIT_KEY which is 'F1'. When pressed anytime the game quits.
}

void Game::UpdateMapSelection(float deltaTime)
{
	if (App::IsKeyPressed('1'))
	{
		m_selectedWorld = WorldType::EARTH;
		*m_currentGameState = GameState::PLAYING;
	}
	if (App::IsKeyPressed('2'))
	{
		m_selectedWorld = WorldType::MARS;
		*m_currentGameState = GameState::PLAYING;
	}
	if (App::IsKeyPressed('3'))
	{
		m_selectedWorld = WorldType::SUPER_EARTH;
		*m_currentGameState = GameState::PLAYING;
	}

	// Return to main menu
	if (App::IsKeyPressed(VK_ESCAPE))
	{
		*m_currentGameState = GameState::MENU;
	}
}

void Game::UpdateGame(const float deltaTime)
{
	if (!m_isWorldInitialized) InitializeMap(m_selectedWorld, m_currentGameState, m_score);
	if (m_isScoreUpToDate) m_isScoreUpToDate = false; // Since game is in progress the Game::score won't be up-to-date

	// While playing press ESC to pause
	if (App::IsKeyPressed(VK_ESCAPE))
	{
		*m_currentGameState = GameState::PAUSED;
	}
	m_game->Update(deltaTime);
}

void Game::UpdatePaused(float deltaTime)
{
	// While paused ENTER to play
	if (App::IsKeyPressed(VK_RETURN))
	{
		*m_currentGameState = GameState::PLAYING;
	}
	// While paused press SPACE BAR to main menu
	if (App::IsKeyPressed(VK_SPACE))
	{
		*m_currentGameState = GameState::MENU;
		m_isWorldInitialized = false;
		m_game.reset();
	}
}

void Game::UpdateGameOver(float deltaTime)
{
	// Update the score before closing the game
	if (!m_isScoreUpToDate)
	{
		m_isScoreUpToDate = true;
	}

	// While game over press SPACE BAR or ESC to main menu
	if (App::IsKeyPressed(VK_SPACE) || App::IsKeyPressed(VK_ESCAPE))
	{
		*m_currentGameState = GameState::MENU;
		m_isWorldInitialized = false;
		m_game.reset();
	}
}

void Game::Render() const
{
	switch (*m_currentGameState)
	{
		case GameState::MENU:
			RenderMenu();
			break;
		case GameState::MAP_SELECT:
			RenderMapSelection();
			break;
		case GameState::PLAYING:
			RenderGame();
			break;
		case GameState::PAUSED:
			RenderPaused();
			break;
		case GameState::GAME_OVER:
			RenderGameOver();
			break;
	}
}

void Game::RenderMenu() const
{
	RenderUILayout();
	RenderUIControls();

	float currentY = m_menuStartY;
	const std::vector<std::pair<std::string, std::string>> menuItems = {
		{"PLAY GAME", "[Enter]"},
		// {"CREDITS", "[2]"},
		{"EXIT", "[F1]"}
	};

	Graphics::PrintText(
		"Experience golf in other planets!",
		Vector2(m_leftMargin, currentY + 60.f),
		m_fontPrimary,
		GLUT_BITMAP_HELVETICA_18
	);

	Graphics::DrawLine(
		Vector2(m_leftMargin - 10, currentY + 30),
		Vector2(m_leftMargin + 300, currentY + 30),
		m_accentPrimary
	);

	for (const auto& [text, shortcut] : menuItems)
	{
		Graphics::PrintText(
			text,
			Vector2(m_leftMargin, currentY),
			m_fontPrimary,
			GLUT_BITMAP_HELVETICA_18
		);
		if (!shortcut.empty())
		{
			Graphics::PrintText(
				shortcut,
				Vector2(m_leftMargin + 130, currentY),
				m_fontSecondary,
				GLUT_BITMAP_HELVETICA_18
			);
		}
		currentY -= m_menuSpacing;
	}
}

void Game::RenderMapSelection() const
{
	RenderUILayout();

	constexpr float centerX = Physics::SCREEN_WIDTH / 2.f;
	constexpr float startY = Physics::SCREEN_HEIGHT / 2.f;

	Graphics::PrintText(
		"SELECT WORLD!",
		Vector2(centerX - 80.f, startY + 150.f),
		m_fontPrimary,
		GLUT_BITMAP_TIMES_ROMAN_24
	);

	Graphics::DrawLine(
		Vector2(centerX - 150, startY + 80),
		Vector2(centerX + 150, startY + 80),
		m_accentPrimary
	);

	const std::vector<std::pair<std::string, std::string>> menuItems = {
		{"EARTH", "[1]"},
		{"MARS (Low Gravity)", "[2]"},
		{"Super Earth (Extra gravity)", "[3]"}
	};

	float currentY = startY + 50;
	for (const auto& [text, shortcut] : menuItems)
	{
		Graphics::PrintText(
			text,
			Vector2(centerX - 140, currentY),
			m_fontSecondary,
			GLUT_BITMAP_HELVETICA_18
		);
		if (!shortcut.empty())
		{
			Graphics::PrintText(
				shortcut,
				Vector2(centerX + 100, currentY),
				m_fontPrimary,
				GLUT_BITMAP_HELVETICA_18
			);
		}
		currentY -= m_menuSpacing;
	}

}

void Game::RenderGame() const
{
	if (m_game) m_game->Render();
}

void Game::RenderPaused() const
{
	RenderUILayout();
	RenderUIControls();

	constexpr float centerX = Physics::SCREEN_WIDTH / 2.f;
	constexpr float startY = Physics::SCREEN_HEIGHT / 2.f;

	// Pause symbols
	for (const float offset : {-20.f, 20.f})
	{
		Graphics::DrawFillRectangle(
			Vector2(centerX + offset, startY + 200.f),
			20.0f, 60.0f,
			m_accentPrimary
		);
	}

	Graphics::PrintText(
		"GAME PAUSED!",
		Vector2(centerX - 80.f, startY + 150.f),
		m_fontPrimary,
		GLUT_BITMAP_TIMES_ROMAN_24
	);

	Graphics::PrintText(
		"Plan your shot by analysing the world characteristics. Wind speed and gravity is displayed in the HUD but the friction and bounciness are hidden. ",
		Vector2(centerX - 380.f, startY + 100.f),
		m_fontPrimary,
		GLUT_BITMAP_HELVETICA_12
	);

	Graphics::DrawLine(
		Vector2(centerX - 150, startY + 80),
		Vector2(centerX + 150, startY + 80),
		m_accentPrimary
	);

	const std::vector<std::pair<std::string, std::string>> menuItems = {
		{"RESUME GAME", "[Enter]"},
		{"MAIN MENU", "[Space]"},
		{"QUIT", "[F1]"}
	};

	float currentY = startY + 50;
	for (const auto& [text, shortcut] : menuItems)
	{
		Graphics::PrintText(
			text,
			Vector2(centerX - 140, currentY),
			m_fontSecondary,
			GLUT_BITMAP_HELVETICA_18
		);
		if (!shortcut.empty())
		{
			Graphics::PrintText(
				shortcut,
				Vector2(centerX + 70, currentY),
				m_fontPrimary,
				GLUT_BITMAP_HELVETICA_18
			);
		}
		currentY -= m_menuSpacing;
	}
}

void Game::RenderGameOver() const
{
	RenderUILayout();

	constexpr float centerX = Physics::SCREEN_WIDTH / 2.f;
	float currentY = Physics::SCREEN_HEIGHT / 2.f;

	Graphics::PrintText(
		"GAME OVER!",
		Vector2(centerX - 80.f, currentY + 140.f),
		m_accentPrimary,
		GLUT_BITMAP_TIMES_ROMAN_24
	);

	Graphics::DrawLine(
		Vector2(centerX - 150, currentY + 30),
		Vector2(centerX + 150, currentY + 30),
		m_accentPrimary
	);

	const std::pair<std::string, std::string> headers = { "PLAYER", "TOTAL STROKES" };
	Graphics::PrintText(
		headers.first,
		Vector2(centerX - 140, currentY),
		m_fontPrimary,
		GLUT_BITMAP_HELVETICA_18
	);
	Graphics::PrintText(
		headers.second,
		Vector2(centerX - 10, currentY),
		m_fontSecondary,
		GLUT_BITMAP_HELVETICA_18
	);

	currentY -= m_menuSpacing;
	Graphics::PrintText(
		"Player 1",
		Vector2(centerX - 140, currentY),
		m_fontPrimary,
		GLUT_BITMAP_9_BY_15
	);
	Graphics::PrintText(
		std::to_string(m_score->playerOneTotalShots),
		Vector2(centerX + 60, currentY),
		m_fontSecondary,
		GLUT_BITMAP_9_BY_15
	);
}

void Game::RenderUILayout() const
{
	// Background
	UIEffects::RenderFadingBackground(m_background, 2.5f);
	UIEffects::RenderStartField(m_accentSecondary, 100, 12345);

	// Concentric background circles
	const Vector2 circleCenter(
		Physics::SCREEN_WIDTH - (Physics::SCREEN_WIDTH / 4.0f),
		Physics::SCREEN_HEIGHT / 2.0f
	);

	for (int i = 0; i < 4; i++)
	{
		const float radius = 100.0f + (static_cast<float>(i) * 50.0f);
		const float alpha = 0.3f - (static_cast<float>(i) * 0.05f);
		Graphics::DrawCircle(circleCenter, radius, 36, Color(alpha, 0.0f, 0.0f));
	}

	// Decorations
	// Top-left circles
	UIEffects::DrawConcentricCircles(
		{ m_leftMargin - 20, Physics::SCREEN_HEIGHT - 50 },
		3, 20.f, 10.f, 6, m_accentSecondary
	);

	// Bottom-right triangle
	const std::vector<Vector2> triangle = {
		{Physics::SCREEN_WIDTH - 100, 50},
		{Physics::SCREEN_WIDTH - 50, 100},
		{Physics::SCREEN_WIDTH - 150, 100}
	};
	Graphics::DrawPolygon(triangle, m_accentSecondary);

	// Title background
	const std::vector<Vector2> titleBox = {
		{m_leftMargin - 20, m_titleY + 30},
		{m_leftMargin + 200, m_titleY + 30},
		{m_leftMargin + 220, m_titleY - 10},
		{m_leftMargin - 40, m_titleY - 10}
	};
	Graphics::DrawPolygon(titleBox, m_fontTitle);

	// Title 
	Graphics::PrintText(
		"GALAXY GOLF",
		Vector2(m_leftMargin, m_titleY),
		m_fontTitle,
		GLUT_BITMAP_TIMES_ROMAN_24
	);
}

void Game::RenderUIControls() const
{
	float currentY = m_controlsStartY;

	Graphics::DrawLine(
		Vector2(m_leftMargin - 10, currentY + 30),
		Vector2(m_leftMargin + 300, currentY + 30),
		m_accentPrimary
	);

	Graphics::PrintText(
		"CONTROLS",
		Vector2(m_leftMargin, currentY),
		Color(Colors::GRAY),
		GLUT_BITMAP_HELVETICA_18
	);

	const std::vector<std::pair<std::string, std::string>> controls = {
		{"Aim", "Hold Left mouse button and drag"},
		{"Shoot", "Release the left mouse button"},
		{"Weak Shot", "Q"},
		{"Normal Shot", "W"},
		{"Power Shot", "E"}
	};

	currentY -= m_controlsSpacing / 2.f;

	for (const auto& [action, key] : controls)
	{
		currentY -= m_controlsSpacing;
		Graphics::PrintText(
			action,
			Vector2(m_leftMargin, currentY),
			m_fontPrimary,
			GLUT_BITMAP_9_BY_15
		);
		Graphics::PrintText(
			key,
			Vector2(m_leftMargin + 120, currentY),
			m_fontSecondary,
			GLUT_BITMAP_9_BY_15
		);
	}
}

void Game::Shutdown()
{
	if (m_game)
	{
		m_game->Shutdown();
		m_game.reset();
	}

	Logger::Warn("Game::Shutdown(): " + std::to_string(m_score->playerOneTotalShots));

}
