#pragma once

#include <vector>

#include "Constants.h"
#include "src/Utils/Matrix.h"
#include "src/Utils/Vector2.h"

class Camera
{
public:
	/**
	 * Constructs a camera with specified viewport dimensions.
	 *
	 * @param width The width of the viewport (defaults to screen width)
	 * @param height The height of the viewport (defaults to screen height)
	 */
	Camera(const float width = Physics::SCREEN_WIDTH, const float height = Physics::SCREEN_HEIGHT);

	/**
	 * Transforms a world position to screen coordinates.
	 *
	 * @param worldPos The position in world space
	 * @param camera The camera performing the transformation
	 * @return Vector2 The position in screen space
	 */
	static Vector2 WorldToScreen(const Vector2& worldPos, const Camera& camera);

	/**
	* Transforms a vector of vertices from world to screen space.
	*
	* @param vertices Vector of vertices in world space
	* @param camera The camera performing the transformation
	* @return std::vector<Vector2> Transformed vertices in screen space
	*/
	static std::vector<Vector2> TransformVertices(const std::vector<Vector2>& vertices, const Camera& camera);

	/**
	 * Updates the projection matrix based on current viewport dimensions.
	 *
	 * Creates an orthographic projection matrix using the current viewport boundaries.
	 * This matrix transforms from view space to clip space.
	 */
	void UpdateProjectionMatrix();

	/**
	 * @brief Updates the viewport dimensions.
	 *
	 * @param width New viewport width
	 * @param height New viewport height
	 */
	void SetViewport(float width, float height);

	void SetPosition(const Vector2& newPosition);

	void SetPosition(float x, float y);

	void Move(const Vector2& offset);

	void Move(float dx, float dy);

	const Vector2& GetPosition() const;

private:
	// Orthographic projection boundaries
	float m_left;		// Left boundary of the screen view
	float m_right;		// Right boundary of the screen view
	float m_bottom;		// Bottom boundary of the screen view
	float m_top;		// Top boundary of the screen view
	float m_zNear;		// Near clipping plane
	float m_zFar;		// Far clipping plane

	// Transformation matrices for camera positioning and projection
	Matrix m_projectionMatrix;	// Transforms from view space to clip space
	Matrix m_viewMatrix;		// Transforms from world space to view space
	Vector2 m_position;			// Camera's position in world space

	/**
	 * Updates the view matrix based on camera position.
	 *
	 * Creates a view matrix that transforms from world space to view space
	 * by translating by the negative of the camera's position.
	 */
	void UpdateViewMatrix();

	[[nodiscard]] const Matrix& GetProjectionMatrix() const;
	[[nodiscard]] const Matrix& GetViewMatrix() const;
};
