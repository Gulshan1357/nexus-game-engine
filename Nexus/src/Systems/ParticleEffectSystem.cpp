#include "stdafx.h"

#include "ParticleEffectSystem.h"

#include "App/AppSettings.h"

#include "src/ECS/Coordinator.h"
#include "src/Components/TransformComponent.h"
#include "src/Components/ParticleEmitterComponent.h"

#include "src/Physics/Constants.h"
#include "src/Physics/Particle.h"
#include "src/Utils/Vector2.h"
#include "src/Utils/GraphicsUtils.h"
#include "src/Utils/Math.h"
#include "src/Utils/Random.h"

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

void ParticleEffectSystem::Render() const
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

				Graphics::DrawLine(start, end, color);
				break;
			}

			case ParticleShape::CIRCLE:
				Graphics::DrawFillCircle(particle.position, size / 2.f, 6, color);
				break;

			case ParticleShape::SQUARE:
			{
				const float halfSize = size / 2.0f;
				const float cosAngle = std::cos(particle.rotation);
				const float sinAngle = std::sin(particle.rotation);

				// Calculate rotated corners
				std::vector<Vector2> corners = {
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

				Graphics::DrawFillPolygon(corners, color);
				break;
			}
		}
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