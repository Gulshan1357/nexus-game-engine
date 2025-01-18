#include "stdafx.h"

#include "ParticleEffectSystem.h"

#include <functional>

#include "App/AppSettings.h"

#include "src/ECS/Coordinator.h"
#include "src/EventManagement/EventManager.h"
#include "src/Events/PlayerStateChangeEvent.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/ParticleEmitterComponent.h"

#include "src/Physics/Constants.h"
#include "src/Physics/Particle.h"
#include "src/Utils/Vector2.h"
#include "src/Utils/GraphicsUtils.h"
#include "src/Utils/Math.h"
#include "src/Utils/Random.h"
#include "src/Physics/Camera.h"

void ParticleEffectSystem::Update(const float deltaTime)
{
	UpdateParticle(deltaTime);
	UpdateEmitters(deltaTime);
}

void ParticleEffectSystem::UpdateParticle(const float deltaTime)
{
	for (auto& particle : m_particlePool)
	{
		if (!particle.active)
			continue;

		if (particle.lifeRemaining <= 0.0f)
		{
			particle.active = false;
			continue;
		}

		particle.lifeRemaining -= deltaTime;

		if (particle.useGravity)
		{
			particle.velocity.y -= Physics::gravity * particle.gravityStrength * deltaTime;
		}

		particle.position += particle.velocity * deltaTime;
		if (particle.rotation < 0)
			particle.rotation -= 5.f * deltaTime;  // Keep rotating negative
		else
			particle.rotation += 5.f * deltaTime;  // Keep rotating positive
	}
}

void ParticleEffectSystem::UpdateEmitters(const float deltaTime)
{
	const auto entities = GetSystemEntities();

	for (auto& entity : entities)
	{
		const auto& transform = entity.GetComponent<TransformComponent>();
		auto& emitter = entity.GetComponent<ParticleEmitterComponent>();

		if (!emitter.isActive)
			continue;

		// Calculate how many particles to emit this frame
		emitter.timeSinceLastEmission += deltaTime;
		const float particlesToEmit = emitter.emissionRate * emitter.timeSinceLastEmission;

		const int numParticlesToEmit = static_cast<int>(particlesToEmit);
		if (numParticlesToEmit > 0)
		{
			emitter.timeSinceLastEmission -= static_cast<float>(numParticlesToEmit) / emitter.emissionRate;

			// Emit the particles
			for (int i = 0; i < numParticlesToEmit; i++)
			{
				const Vector2 emissionPos = CalculateEmissionPosition(transform.position, emitter);
				EmitParticle(ParticleProps{
					emissionPos,
					emitter.properties.particleShape,
					emitter.properties.velocity,
					emitter.properties.velocityVariations,
					emitter.properties.colorBegin,
					emitter.properties.colorEnd,
					emitter.properties.sizeBegin,
					emitter.properties.sizeEnd,
					emitter.properties.sizeVariations,
					emitter.properties.lifeTime,
					emitter.properties.useGravity,
					emitter.properties.gravityStrength
					});
			}
		}
	}
}

void ParticleEffectSystem::EmitParticle(const ParticleProps& particleProps)
{
	Particle& particle = m_particlePool[m_poolIndex];

	particle.active = true;
	particle.particleShape = particleProps.particleShape;
	particle.position = particleProps.position;
	particle.rotation = Random::Float(-PI, PI) * 2.0f;

	// Velocity
	particle.velocity = particleProps.velocity;
	particle.velocity.x += particleProps.velocityVariations.x * Random::Float(-0.5f, 0.5f);
	particle.velocity.y += particleProps.velocityVariations.y * Random::Float(-0.5f, 0.5f);

	// Color
	particle.colorBegin = particleProps.colorBegin;
	particle.colorEnd = particleProps.colorEnd;

	// Gravity
	particle.useGravity = particleProps.useGravity;
	particle.gravityStrength = particleProps.gravityStrength;

	particle.lifeTime = particleProps.lifeTime;
	particle.lifeRemaining = particleProps.lifeTime;
	particle.sizeBegin = particleProps.sizeBegin + particleProps.sizeVariations * Random::Float(-0.5f, 0.5f);
	particle.sizeEnd = particleProps.sizeEnd;

	m_poolIndex = (m_poolIndex - 1) % m_particlePool.size();
}

void ParticleEffectSystem::Render(const Camera& camera) const
{
	for (const auto& particle : m_particlePool)
	{
		if (!particle.active)
			continue;

		// Lerp particles w.r.t life
		const float life = particle.lifeRemaining / particle.lifeTime;

		Color color = Math::Lerp(particle.colorEnd, particle.colorBegin, life);
		const float size = Math::Lerp(particle.sizeEnd, particle.sizeBegin, life);

		switch (particle.particleShape)
		{
			case ParticleShape::LINE:
			{
				// Calculate rotated line endpoints
				const float halfSize = size / 2.0f;
				const float cosAngle = std::cos(particle.rotation);
				const float sinAngle = std::sin(particle.rotation);

				// Calculate rotated endpoints
				Vector2 start(
					particle.position.x - halfSize * cosAngle,
					particle.position.y - halfSize * sinAngle
				);
				Vector2 end(
					particle.position.x + halfSize * cosAngle,
					particle.position.y + halfSize * sinAngle
				);

				start = Camera::WorldToScreen(start, camera);
				end = Camera::WorldToScreen(end, camera);

				Graphics::DrawLine(start, end, color);
				break;
			}

			case ParticleShape::CIRCLE:
			{
				const Vector2 screenPos = Camera::WorldToScreen(particle.position, camera);
				Graphics::DrawFillCircle(screenPos, size / 2.f, 6, color);
				break;
			}

			case ParticleShape::SQUARE:
			{
				const float halfSize = size / 2.0f;
				const float cosAngle = std::cos(particle.rotation);
				const float sinAngle = std::sin(particle.rotation);

				// Calculate rotated corners
				std::vector<Vector2> worldVertex = {
					// Top-left
					Vector2(
						particle.position.x - halfSize * cosAngle + halfSize * sinAngle,
						particle.position.y - halfSize * sinAngle - halfSize * cosAngle
					),
						// Top-right
						Vector2(
							particle.position.x + halfSize * cosAngle + halfSize * sinAngle,
							particle.position.y + halfSize * sinAngle - halfSize * cosAngle
						),
						// Bottom-right
						Vector2(
							particle.position.x + halfSize * cosAngle - halfSize * sinAngle,
							particle.position.y + halfSize * sinAngle + halfSize * cosAngle
						),
						// Bottom-left
						Vector2(
							particle.position.x - halfSize * cosAngle - halfSize * sinAngle,
							particle.position.y - halfSize * sinAngle + halfSize * cosAngle
						)
				};

				std::vector<Vector2> screenVertex;
				screenVertex.reserve(worldVertex.size());
				for (const auto& vertex : worldVertex)
				{
					screenVertex.push_back(Camera::WorldToScreen(vertex, camera));
				}

				Graphics::DrawFillPolygon(screenVertex, color);
				break;
			}
		}
	}
}

void ParticleEffectSystem::SubscribeToEvents(const std::unique_ptr<EventManager>& eventManager)
{
	using CallbackType = std::function<void(ParticleEffectSystem*, PlayerStateChangeEvent&)>;
	const CallbackType callback = [this](auto&&, auto&& placeholder2) { OnPlayerStateChange(std::forward<decltype(placeholder2)>(placeholder2)); };
	eventManager->SubscribeToEvent<PlayerStateChangeEvent>(this, callback);
	//  std::placeholders::_2 tells std::bind that the second argument (the event) will be provided when the resulting function is called. This allows us to create a callable object that matches the required function signature of SubscribeToEvent, where the first argument is the instance (PhysicsSystem*), and the second argument is the event (CollisionEvent&).
}

void ParticleEffectSystem::OnPlayerStateChange(const PlayerStateChangeEvent& event)
{

	Logger::Log("ParticleEffectSystem receives PlayerStateChangeEvent");

	if (event.isMoving)
	{
		switch (event.activeAbility)
		{
			case Ability::NORMAL_SHOT:
				PlayNormalShotEffect(event.player);
				break;
			case Ability::POWER_SHOT:
				PlayPowerShotEffect(event.player);
				break;
			case Ability::SNIPER_SHOT:
				PlaySniperShotEffect(event.player);
				break;
		}
	}
	else
	{
		PlayIdleEffect(event.player);
	}

}

Vector2 ParticleEffectSystem::CalculateEmissionPosition(const Vector2 emitterPos, const ParticleEmitterComponent& emitter)
{
	// If EmissionShape is CIRCLE then randomize the emitterPos w.r.t emitter radius
	if (emitter.emissionShape == EmissionShape::CIRCLE)
	{
		const float angle = Random::Float() * 2.0f * PI;
		const float radius = Random::Float() * emitter.emissionRadius;
		return emitterPos + Vector2(
			cos(angle) * radius,
			sin(angle) * radius
		);
	}

	return emitterPos;
}

void ParticleEffectSystem::PlayIdleEffect(const Entity& entity)
{
	if (!entity.HasComponent<ParticleEmitterComponent>())
		return;

	auto& particleEmitter = entity.GetComponent<ParticleEmitterComponent>();

	// Regular movement - white meteor effect
	ParticleProps idleParticle;
	idleParticle.particleShape = ParticleShape::CIRCLE;
	idleParticle.colorBegin = Color(0.95f, 0.95f, 1.0f);    // Almost white with slight blue tint
	idleParticle.colorEnd = Color(0.85f, 0.85f, 0.95f);     // Slightly darker/blueish
	idleParticle.sizeBegin = 2.5f;
	idleParticle.sizeEnd = 0.5f;
	idleParticle.sizeVariations = 0.3f;
	idleParticle.lifeTime = 1.5f;
	idleParticle.velocity = { 0.0f, 30.0f };
	idleParticle.velocityVariations = { 15.0f, 15.0f };
	idleParticle.position = { 0.0f, 0.0f };
	idleParticle.useGravity = false;
	idleParticle.gravityStrength = 5.0f;

	particleEmitter.properties = idleParticle;
	particleEmitter.emissionRate = 2.f;
	particleEmitter.isActive = true;
	particleEmitter.emissionShape = EmissionShape::CIRCLE;
	particleEmitter.emissionRadius = 20.f;
	particleEmitter.timeSinceLastEmission = 0.0f;
}

void ParticleEffectSystem::PlayNormalShotEffect(const Entity& entity)
{
	if (!entity.HasComponent<ParticleEmitterComponent>())
		return;

	auto& particleEmitter = entity.GetComponent<ParticleEmitterComponent>();

	// Regular movement - white meteor effect
	ParticleProps normalShotParticle;
	normalShotParticle.particleShape = ParticleShape::CIRCLE;
	normalShotParticle.colorBegin = Color(1.0f, 1.0f, 1.0f);      // Pure white
	normalShotParticle.colorEnd = Color(0.85f, 0.85f, 0.9f);      // Light blue-white
	normalShotParticle.sizeBegin = 8.0f;
	normalShotParticle.sizeEnd = 0.5f;
	normalShotParticle.sizeVariations = 0.5f;
	normalShotParticle.lifeTime = 0.6f;
	normalShotParticle.velocity = { 0.0f, 0.0f };
	normalShotParticle.velocityVariations = { 10.0f, 10.0f };
	normalShotParticle.position = { 0.0f, 0.0f };
	normalShotParticle.useGravity = false;

	particleEmitter.properties = normalShotParticle;
	particleEmitter.emissionRate = 20.f;
	particleEmitter.isActive = true;
	particleEmitter.emissionShape = EmissionShape::POINT;
	particleEmitter.emissionRadius = 0.f;
	particleEmitter.timeSinceLastEmission = 0.0f;
}

void ParticleEffectSystem::PlayPowerShotEffect(const Entity& entity)
{
	if (!entity.HasComponent<ParticleEmitterComponent>())
		return;

	auto& particleEmitter = entity.GetComponent<ParticleEmitterComponent>();

	// Power shot - intense red effect
	ParticleProps powerShotParticle;
	powerShotParticle.particleShape = ParticleShape::CIRCLE;
	powerShotParticle.colorBegin = Color(1.0f, 0.8f, 0.3f);       // Bright orange-yellow
	powerShotParticle.colorEnd = Color(0.9f, 0.15f, 0.0f);        // Deep red
	powerShotParticle.sizeBegin = 15.0f;
	powerShotParticle.sizeEnd = 0.8f;
	powerShotParticle.sizeVariations = 0.5f;
	powerShotParticle.lifeTime = 0.8f;
	powerShotParticle.velocity = { 0.0f, 10.0f };
	powerShotParticle.velocityVariations = { 50.0f, 50.0f };
	powerShotParticle.position = { 0.0f, 0.0f };
	powerShotParticle.useGravity = false;

	particleEmitter.properties = powerShotParticle;
	particleEmitter.emissionRate = 20.f;
	particleEmitter.isActive = true;
	particleEmitter.emissionShape = EmissionShape::POINT;
	particleEmitter.emissionRadius = 0.f;
	particleEmitter.timeSinceLastEmission = 0.0f;
}

void ParticleEffectSystem::PlaySniperShotEffect(const Entity& entity)
{
	if (!entity.HasComponent<ParticleEmitterComponent>())
		return;

	auto& particleEmitter = entity.GetComponent<ParticleEmitterComponent>();

	// Sniper shot - precise purple effect
	ParticleProps sniperShotParticle;
	sniperShotParticle.particleShape = ParticleShape::SQUARE;
	sniperShotParticle.colorBegin = Color(0.4f, 0.75f, 1.0f);     // Almost white with slight blue tint
	sniperShotParticle.colorEnd = Color(0.0f, 0.2f, 0.6f);        // Deep dark blue
	sniperShotParticle.sizeBegin = 10.0f;
	sniperShotParticle.sizeEnd = 0.5f;
	sniperShotParticle.sizeVariations = 0.5f;
	sniperShotParticle.lifeTime = 1.0f;
	sniperShotParticle.velocity = { 0.0f, 0.0f };
	sniperShotParticle.velocityVariations = { 10.0f, 10.0f };
	sniperShotParticle.position = { 0.0f, 0.0f };
	sniperShotParticle.useGravity = false;

	particleEmitter.properties = sniperShotParticle;
	particleEmitter.emissionRate = 20.f;
	particleEmitter.isActive = true;
	particleEmitter.emissionShape = EmissionShape::POINT;
	particleEmitter.emissionRadius = 0.f;
	particleEmitter.timeSinceLastEmission = 0.0f;
}
