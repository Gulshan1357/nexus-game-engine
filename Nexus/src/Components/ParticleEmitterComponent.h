#pragma once

#include "src/Physics/Particle.h"

enum class EmissionShape { POINT, CIRCLE };

/**
 * ParticleEmitterComponent provides information for the Particle System.
 * @param properties (ParticleProps) Properties of the particle
 * @param emissionRate (Float) Particles per second
 * @param isActive (Bool)
 * @param emissionShape (EmissionShape) Whether to emit the particle from a point or randomly within a circle.
 * @param emissionRadius (Float) Radius of emission area if the emissionShape is Circle
 * @param timeSinceLastEmission (Float)
*/
struct ParticleEmitterComponent
{
	ParticleProps properties;
	float emissionRate;
	bool isActive;
	EmissionShape emissionShape;
	float emissionRadius;
	float timeSinceLastEmission;

	explicit ParticleEmitterComponent(
		const ParticleProps& properties,
		const float emissionRate = 10.0f,
		const bool isActive = true,
		const EmissionShape emissionShape = EmissionShape::POINT,
		const float emissionRadius = 1.0f,
		const float timeSinceLastEmission = 0.0f)
		: properties(properties),
		emissionRate(emissionRate),
		isActive(isActive),
		emissionShape(emissionShape),
		emissionRadius(emissionRadius),
		timeSinceLastEmission(timeSinceLastEmission)
	{}
};
