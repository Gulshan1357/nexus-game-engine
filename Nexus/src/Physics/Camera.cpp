#include "stdafx.h"
#include "Camera.h"


Camera::Camera(const float width /*= Physics::SCREEN_WIDTH*/, const float height /*= Physics::SCREEN_HEIGHT*/) : m_left(-width * 0.5f)
, m_right(width * 0.5f)
, m_bottom(-height * 0.5f)
, m_top(height * 0.5f)
, m_zNear(-1.0f)
, m_zFar(1.0f)
, m_projectionMatrix(4, 4)
, m_viewMatrix(4, 4)
, m_position(0.0f, 0.0f)
{
	UpdateProjectionMatrix();

	// Initialize view matrix as identity
	m_viewMatrix.Zero();
	for (size_t i = 0; i < 4; i++)
	{
		m_viewMatrix.rowVectors[i][i] = 1.0f;
	}
}

Vector2 Camera::WorldToScreen(const Vector2& worldPos, const Camera& camera)
{
	// Preallocate vectors to avoid stack allocations
	static VectorN point(4);
	static VectorN viewPos(4);
	static VectorN clipPos(4);

	// Convert to homogeneous coordinates (x, y, z, w)
	point[0] = worldPos.x;
	point[1] = worldPos.y;
	point[2] = 0.0f;	// Z-coordinate (2D, so we use 0)
	point[3] = 1.0f;	// W-coordinate for homogeneous coordinates

	// Apply view and projection transformations
	viewPos = camera.GetViewMatrix() * point;
	clipPos = camera.GetProjectionMatrix() * viewPos;

	// Convert from clip space to screen space
	return {
		(clipPos[0] + 1.0f) * (APP_VIRTUAL_WIDTH * 0.5f),	// Map from [-1,1] to [0,width]
		(clipPos[1] + 1.0f) * (APP_VIRTUAL_HEIGHT * 0.5f)	// Map from [-1,1] to [0,height]
	};
}

std::vector<Vector2> Camera::TransformVertices(const std::vector<Vector2>& vertices, const Camera& camera)
{
	std::vector<Vector2> transformedVertices;
	transformedVertices.reserve(vertices.size());

	for (const auto& vertex : vertices)
	{
		Vector2 screenPos = WorldToScreen(vertex, camera);
		transformedVertices.emplace_back(screenPos.x, screenPos.y);
	}
	return transformedVertices;
}

void Camera::UpdateProjectionMatrix()
{
	m_projectionMatrix.Zero();

	// Build orthographic projection matrix:
	// [2/(r-l)      0           0       -(r+l)/(r-l)]
	// [0            2/(t-b)     0       -(t+b)/(t-b)]
	// [0            0           -2/(f-n) -(f+n)/(f-n)]
	// [0            0           0        1          ]

	m_projectionMatrix.rowVectors[0][0] = 2.0f / (m_right - m_left);
	m_projectionMatrix.rowVectors[0][3] = -(m_right + m_left) / (m_right - m_left);

	m_projectionMatrix.rowVectors[1][1] = 2.0f / (m_top - m_bottom);
	m_projectionMatrix.rowVectors[1][3] = -(m_top + m_bottom) / (m_top - m_bottom);

	m_projectionMatrix.rowVectors[2][2] = -2.0f / (m_zFar - m_zNear);
	m_projectionMatrix.rowVectors[2][3] = -(m_zFar + m_zNear) / (m_zFar - m_zNear);

	m_projectionMatrix.rowVectors[3][3] = 1.0f;
}

void Camera::SetViewport(float width, float height)
{
	m_right = width;
	m_top = height;
	UpdateProjectionMatrix();
}

void Camera::SetPosition(const Vector2& newPosition)
{
	m_position = newPosition;
	UpdateViewMatrix();
}

void Camera::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
	UpdateViewMatrix();
}

void Camera::Move(const Vector2& offset)
{
	m_position += offset;
	UpdateViewMatrix();
}

void Camera::Move(float dx, float dy)
{
	m_position.x += dx;
	m_position.y += dy;
	UpdateViewMatrix();
}

const Vector2& Camera::GetPosition() const
{
	return m_position;
}

void Camera::UpdateViewMatrix()
{
	m_viewMatrix.Zero();
	m_viewMatrix.rowVectors[0][0] = 1.0f;
	m_viewMatrix.rowVectors[1][1] = 1.0f;
	m_viewMatrix.rowVectors[2][2] = 1.0f;
	m_viewMatrix.rowVectors[3][3] = 1.0f;
	m_viewMatrix.rowVectors[0][3] = -m_position.x;
	m_viewMatrix.rowVectors[1][3] = -m_position.y;
}

const Matrix& Camera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

const Matrix& Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}
