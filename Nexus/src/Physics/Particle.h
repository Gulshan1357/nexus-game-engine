#pragma once

#include "src/Utils/Vector2.h"
#include "src/Utils/Color.h"

enum class ParticleShape { LINE, CIRCLE, SQUARE };

// A struct to store the properties of a particle. The Particle System will emit the particle based on these values.
struct ParticleProps
{
	Vector2 position = {};
	ParticleShape particleShape = ParticleShape::CIRCLE;
	Vector2 velocity = {}, velocityVariations = {};
	Color colorBegin = {}, colorEnd = {};
	float sizeBegin = {}, sizeEnd = {}, sizeVariations = {}; // Size(Diameter/length) of the particle
	float lifeTime = 1.0f; // In seconds

	bool useGravity = false;
	float gravityStrength = 1.0f;
};

// A particle that is emitted, updated and destroyed by the Particle System.
struct Particle
{
	Vector2 position = {};
	ParticleShape particleShape = ParticleShape::CIRCLE;
	Vector2 velocity = {};
	Color colorBegin = {}, colorEnd = {};
	float rotation = {};
	float sizeBegin = {}, sizeEnd = {};

	float lifeTime = 1.0f; // In seconds
	float lifeRemaining = 0.0f;

	bool useGravity = false;
	float gravityStrength = 1.0f;

	bool active = false;
};