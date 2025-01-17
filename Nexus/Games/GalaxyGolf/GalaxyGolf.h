#pragma once

#include <memory>
#include <string>

#include "src/InputManagement/InputEnums.h"
#include "src/Physics/Constants.h"
#include "src/Physics/Camera.h"

enum class MapType;
enum class ColliderType;
class Coordinator;
class EventManager;
class InputManager;
class AssetManager;
class AudioManager;
struct Vector2;

class GalaxyGolf
{
public:
	GalaxyGolf(MapType mapType);
	~GalaxyGolf();

	void Initialize(MapType mapType);
	void LoadLevel(int level);
	void Update(float deltaTime);
	void ProcessInput();
	void Render();
	void Shutdown();

private:
	//------------------------------------------------------------------------
	// Engine
	//------------------------------------------------------------------------
	bool m_isDebug; // Trigger debug mode using 'B' key
	bool m_bWasLMousePressedPast = false;

	std::unique_ptr<Coordinator> m_coordinator;
	std::unique_ptr<EventManager> m_eventManager;
	std::unique_ptr<InputManager> m_inputManager;
	std::unique_ptr<AssetManager> m_assetManager;
	std::unique_ptr<AudioManager> m_audioManager;

	//------------------------------------------------------------------------
	// World
	//------------------------------------------------------------------------

	float G = -Physics::gravity;
	Camera m_camera;

	MapType m_mapType;
};
