#pragma once

#include "src/ECS/Coordinator.h"

class EventManager;
class PlayerStateChangeEvent;
class Camera;
struct Vector2;
struct TransformComponent;
struct ParticleEmitterComponent;
struct Particle;
struct ParticleProps;

class ParticleEffectSystem : public System
{
public:
	ParticleEffectSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<ParticleEmitterComponent>();
		m_particlePool.resize(1000);
	}

	// Update existing particles and emit new particles based on the ParticleEmitterComponent.
	// DeltaTime is in seconds
	void Update(const float deltaTime);

	// Update all the existing particles
	void UpdateParticle(const float deltaTime);

	// Emit new particles based on the ParticleEmitterComponent
	void UpdateEmitters(const float deltaTime);

	// Emit a single Particle based on the ParticleProps
	void EmitParticle(const ParticleProps& particleProps);

	// Render all the exiting particles
	void Render(const Camera& camera) const;

	// Galaxy Golf Game: Subscribe to Player State change event
	void SubscribeToEvents(const std::shared_ptr<EventManager>& eventManager);
	static void OnPlayerStateChange(const PlayerStateChangeEvent& event);

private:
	std::vector<Particle> m_particlePool;
	size_t m_poolIndex = 999;

	static Vector2 CalculateEmissionPosition(const Vector2 emitterPos, const ParticleEmitterComponent& emitter);

	// Galaxy Golf Game: Particle effects for different shots
	static void PlayIdleEffect(const Entity& entity);
	static void PlayNormalShotEffect(const Entity& entity);
	static void PlayPowerShotEffect(const Entity& entity);
	static void PlaySniperShotEffect(const Entity& entity);
};
