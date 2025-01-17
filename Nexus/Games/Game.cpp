#include "stdafx.h"
#include "Game.h"

#include "App/app.h"

#include "GalaxyGolf/GalaxyGolf.h"

#include "src/Physics/Constants.h"
#include "src/Utils/GraphicsUtils.h"
#include "src/Utils/Vector2.h"
#include "UI/UIEffects.h"

Game::Game() : m_currentState(GameState::MENU)
{}

Game::~Game() = default;

void Game::Initialize()
{
	game = std::make_unique<GalaxyGolf>();
	game->Initialize();
}

void Game::Update(const float deltaTime)
{
	switch (m_currentState)
	{
		case GameState::MENU:
			UpdateMenu(deltaTime);
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

void Game::UpdateMenu(float deltaTime)
{
	// Enter to play
	if (App::IsKeyPressed(VK_RETURN))
	{
		Game::Initialize();
		m_currentState = GameState::PLAYING;
	}

	// App/main.h watch the APP_QUIT_KEY which is 'F1'. When pressed anytime the game quits.
}

void Game::UpdateGame(const float deltaTime)
{
	// While playing press ESC to pause
	if (App::IsKeyPressed(VK_ESCAPE))
	{
		m_currentState = GameState::PAUSED;
	}
	game->Update(deltaTime);
}

void Game::UpdatePaused(float deltaTime)
{
	// While paused ENTER to play
	if (App::IsKeyPressed(VK_RETURN))
	{
		m_currentState = GameState::PLAYING;
	}
	// While paused press SPACEBAR to main menu
	if (App::IsKeyPressed(VK_SPACE))
	{
		m_currentState = GameState::MENU;
	}
}

void Game::UpdateGameOver(float deltaTime)
{
	// While gameover press SPACEBAR or ESC to main menu
	if (App::IsKeyPressed(VK_SPACE) || App::IsKeyPressed(VK_ESCAPE))
	{
		m_currentState = GameState::MENU;
	}
}

void Game::Render() const
{
	switch (m_currentState)
	{
		case GameState::MENU:
			RenderMenu();
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
		{"CREDITS", "[2]"},
		{"EXIT", "[F1]"}
	};

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

void Game::RenderGame() const
{
	game->Render();
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

	const std::pair<std::string, std::string> headers = { "PLAYER", "SCORE" };
	Graphics::PrintText(
		headers.first,
		Vector2(centerX - 140, currentY),
		m_fontPrimary,
		GLUT_BITMAP_HELVETICA_18
	);
	Graphics::PrintText(
		headers.second,
		Vector2(centerX + 60, currentY),
		m_fontSecondary,
		GLUT_BITMAP_HELVETICA_18
	);

	currentY -= m_menuSpacing;
	Graphics::PrintText(
		"playerName",
		Vector2(centerX - 140, currentY),
		m_fontPrimary,
		GLUT_BITMAP_9_BY_15
	);
	Graphics::PrintText(
		std::to_string(13244),
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
		{"Move", "WASD | Left joystick"},
		{"Pause", "Esc | ------"}
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
			Vector2(m_leftMargin + 50, currentY),
			m_fontSecondary,
			GLUT_BITMAP_9_BY_15
		);
	}
}

void Game::Shutdown()
{
	if (game)
	{
		game->Shutdown();
		game.reset();
	}
}
