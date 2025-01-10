#pragma once
#include <vector>

struct Contact;
struct VectorN;
struct Matrix;
struct PolygonColliderComponent;
struct BoxColliderComponent;
struct CircleColliderComponent;
struct TransformComponent;
class RigidBodyComponent;
struct Vector2;
class Entity;

class PhysicsEngine
{
public:

	//------------------------------------------------------------------------
	// Entity setup
	//------------------------------------------------------------------------

	// Calculate inverse mass, angular mass(Moment of Inertia) and inverse angular mass of entity with RigidBody and isKinematic set to false
	static void InitializeEntityPhysics(const Entity& entity);

	// Calculate Angular Mass
	static float CalculateMomentOfInertia(const Entity& entity);


	//------------------------------------------------------------------------
	// Integrating Forces(and Torque) and velocities
	//------------------------------------------------------------------------

	// Integrate Forces to change the rigidbody's linear and angular velocities
	static void IntegrateForces(RigidBodyComponent& rigidBodyComponent, float dt);

	// Integrate linear and angular velocities to change the transform's position and rotation. Make sure to update the properties of collider based on the new values of the transform.
	static void IntegrateVelocities(const RigidBodyComponent& rigidBodyComponent, TransformComponent& transformComponent, float dt);


	//------------------------------------------------------------------------
	// Update Collider w.r.t the transform component
	//------------------------------------------------------------------------

	// Update the properties like globalCenter(circle) and globalVertices(polygon) of the collider w.r.t. the transform component
	static void UpdateColliderProperties(const Entity& entity, TransformComponent& transform);
	// Function to update Circle-Collider's center
	static void UpdateCircleColliderCenter(CircleColliderComponent& circleCollider, const TransformComponent& transform);
	// Function to update Box-Collider's Vertices based on transform rotation
	static void UpdateBoxColliderVertices(BoxColliderComponent& collider, const TransformComponent& transform);
	// Function to update Polygon-Collider's Vertices based on transform rotation
	static void UpdatePolygonColliderVertices(PolygonColliderComponent& collider, const TransformComponent& transform);


	//------------------------------------------------------------------------
	// Force and Torque
	//------------------------------------------------------------------------

	static void AddForce(RigidBodyComponent& rigidBodyComponent, const Vector2& force);
	static void ClearForces(RigidBodyComponent& rigidBodyComponent);
	static void AddTorque(RigidBodyComponent& rigidBodyComponent, const float torque);
	static void ClearTorque(RigidBodyComponent& rigidBodyComponent);


	//------------------------------------------------------------------------
	// Different Forces
	//------------------------------------------------------------------------

	// Drag Force = dragStrength * |v|^2 with direction opposite of velocity, where dragStrength = 1/2 * ρ * K * A
	static Vector2 GenerateDragForce(const RigidBodyComponent& rigidBodyComponent, const float dragStrength);

	// Friction Force = frictionStrength * |v| with direction opposite of velocity, where frictionStrength = μ
	static Vector2 GenerateFrictionForce(const RigidBodyComponent& rigidBodyComponent, const float frictionStrength);

	// Gravitational Force = G * ma * mb / |d|^2 with direction →d, where ma and mb is mass of two objects, d is distance and G is gravitational constant/strength
	static Vector2 GenerateGravitationalForce(const RigidBodyComponent& rigidBodyA, const RigidBodyComponent& rigidBodyB, const Vector2 distanceBMinusA, const float gravitationalStrength, float minDistance, float maxDistance);

	// Spring Force between an entity and anchor = -k * Δl, where k is spring constant(springForceStrength) and Δl is change in spring length
	static Vector2 GenerateSpringForce(const TransformComponent& transformComponent, Vector2 anchor, float restLength, float springForceStrength);

	// Spring Force between two entities = -k * Δl, where k is spring constant(springForceStrength) and Δl is change in spring length
	static Vector2 GenerateSpringForce(const TransformComponent& transformA, const TransformComponent& transformB, float restLength, float springForceStrength);


	//------------------------------------------------------------------------
	// Impulse. An instantaneous change in velocity. Δv = Impulse / mass
	//------------------------------------------------------------------------

	// Apply linear impulse at the Center of Mass
	static void ApplyImpulseLinear(RigidBodyComponent& rigidbody, const Vector2& impulse);
	// Apply angular impulse
	static void ApplyImpulseAngular(RigidBodyComponent& rigidbody, const float impulse);
	// Apply impulse at a certain point which is at distance 'r' from Center of Mass
	static void ApplyImpulseAtPoint(RigidBodyComponent& rigidbody, const Vector2& impulse, const Vector2& r);


	//------------------------------------------------------------------------
	// Collision Detection
	//------------------------------------------------------------------------

	// Axis-Aligned Bounding Box for Collision Detection. Input is the bottom left point of each rectangle along with width and height. Returns true if the two rectangles are colliding.
	static bool IsAABBCollision(const double aX, const double aY, const double aW, const double aH, const double bX, const double bY, const double bW, const double bH);

	// Separating Axis Theorem for Collision Detection between two convex polygon. Input are the list of vertices for each polygon. Returns true if the two polygons are colliding.
	// It also update the outContactPoints vector<Contact> by returning a single contact point for circles and more than one contact points for polygons
	static bool IsSATCollision(const std::vector<Vector2>& verticesA, const std::vector<Vector2>& verticesB, std::vector<Contact>& outContactPoints);


	//------------------------------------------------------------------------
	// Collision Resolution
	//------------------------------------------------------------------------

	// The Projection method just changes the position so that the colliding bodies are just in contact.
	// Displacement for a = depth * (mb) / (ma + mb) = depth * (1/ma) / (1/ma + 1/mb) where 'ma' is mass of 'a' and 'mb' is mass of 'b'
	// This function changes the position
	static void ResolvePenetration(const float depth, const Vector2 collisionNormal, const RigidBodyComponent& aRigidbody, const RigidBodyComponent& bRigidbody, TransformComponent& aTransform, TransformComponent& bTransform);

	// Resolve collision using Impulse Method.
	// This method executes the ResolvePenetration() method, calculate impulse and apply it to the two rigid-bodies.
	// J = -(1 + minimum elasticity) * Relative velocity along collision normal / (1 / Mass of A) + (1 / Mass of B), along the direction of collision normal
	static void ResolveCollision(Vector2 startContactPoint, Vector2 endContactPoint, const float depth, const Vector2 collisionNormal, RigidBodyComponent& aRigidbody, RigidBodyComponent& bRigidbody, TransformComponent& aTransform, TransformComponent& bTransform);

	//------------------------------------------------------------------------
	// Coordinates conversion
	//------------------------------------------------------------------------

	// Convert Local position and rotation to world position and rotation
	static Vector2 LocalSpaceToWorldSpace(const TransformComponent& oldLocalOrigin, const Vector2 pointToConvert);

	// Convert World position and rotation to local position and rotation
	static Vector2 WorldSpaceToLocalSpace(const TransformComponent& newLocalOrigin, const Vector2 pointToConvert);


	//------------------------------------------------------------------------
	// Constraint Resolution
	//------------------------------------------------------------------------

	//---------------------------------------------
	// Matrix6x6 with the all inverse mass and inverse I (angular Mass) of bodies "a" and "b"
	//---------------------------------------------
	//  [ 1/ma  0     0     0     0     0    ]
	//  [ 0     1/ma  0     0     0     0    ]
	//  [ 0     0     1/Ia  0     0     0    ]
	//  [ 0     0     0     1/mb  0     0    ]
	//  [ 0     0     0     0     1/mb  0    ]
	//  [ 0     0     0     0     0     1/Ib ]
	//---------------------------------------------
	static Matrix GetInverseMassMatrix(const RigidBodyComponent& rigidBodyA, const RigidBodyComponent& rigidBodyB);

	//---------------------------------------------
	// VecN with the all linear and angular velocities of bodies "a" and "b"
	//---------------------------------------------
	//  [ va.x ]
	//  [ va.y ]
	//  [ ωa   ]
	//  [ vb.x ]
	//  [ vb.y ]
	//  [ ωb   ]
	//---------------------------------------------
	static VectorN GetVelocitiesVector(const RigidBodyComponent& rigidBodyA, const RigidBodyComponent& rigidBodyB);

	/**
	* @brief Finds the maximum separation between two polygons.
	* Determines the edge of the primary polygon with the largest separation
	* from the secondary polygon along its normal.
	* @param primaryVertices Vertices of the primary polygon.
	* @param secondaryVertices Vertices of the secondary polygon.
	* @param[out] outIndexReferenceEdge Index of the edge with the largest separation.
	* @param[out] outSupportPoint Closest vertex of the secondary polygon.
	* @return Maximum separation value.
	*/
	static float FindMinSeparation(const std::vector<Vector2>& primaryVertices, const std::vector<Vector2>& secondaryVertices, size_t& outIndexReferenceEdge, Vector2& outSupportPoint);

	/**
	 * @brief Finds the edge of the incident polygon closest to the reference 'other' normal.
	 *
	 * Identifies the edge whose normal has the smallest projection onto the reference normal.
	 *
	 * @param verticesIncidentShape Vertices of the incident polygon.
	 * @param outReferenceNormal Reference normal for comparison.
	 * @return Index of the closest edge.
	 */
	static size_t FindIncidentEdge(const std::vector<Vector2>& verticesIncidentShape, const Vector2& outReferenceNormal);

	/**
	 * @brief Clips a line segment against a given line.
	 *
	 * Keeps segment points that are on or behind the clipping line, adding intersection points if needed.
	 *
	 * @param inContacts Endpoints of the segment to clip.
	 * @param[out] outContacts Resulting clipped points.
	 * @param clipVertex0 Start of the clipping line.
	 * @param clipVertex1 End of the clipping line.
	 * @return Number of clipped points.
	 */
	static int ClipSegmentToLine(const std::vector<Vector2>& inContacts, std::vector<Vector2>& outContacts, const Vector2& clipVertex0, const Vector2& clipVertex1);
};