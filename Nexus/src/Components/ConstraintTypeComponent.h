#pragma once

enum class ConstrainType
{
	JOINT
};

/**
 * ConstraintTypeComponent provide information regarding the type of constraint for Constraint System.
 * @param type ConstrainType Enum with possible values = {JOINT}
*/
struct ConstraintTypeComponent
{
	ConstrainType type;

	explicit ConstraintTypeComponent(const ConstrainType type) : type(type) {}
};