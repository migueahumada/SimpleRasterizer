#include "Camera.h"

void Camera::SetLookAt(	const Vector3& eyePos, 
												const Vector3& targetPos, 
												const Vector3& upDir)
{
	m_position = eyePos;
	m_target = targetPos;
	m_up = upDir;
	m_viewMatrix.LookAt(m_position, m_target, m_up);
}

void Camera::SetPerspective(float halfFOV, 
														float widthScreen, 
														float heightScreen, 
														float MinZ, 
														float MaxZ)
{
	m_fov = halfFOV;
	m_width = widthScreen;
	m_height = heightScreen;
	m_minZ = MinZ;
	m_maxZ = MaxZ;

	m_projectionMatrix.Perspective(m_fov, m_width, m_height, m_minZ, m_maxZ);
}


void Camera::Move(const Vector3& direction)
{
	m_position += direction * m_speed;
	//m_target += direction * m_speed;

	bIsDirty = true;
}

void Camera::Rotate(float newYaw, float newPitch)
{
	m_yaw += -newYaw;
	m_pitch += -newPitch;

	//m_pitch = clamp(m_pitch, -89.0f, 89.0f);

	Vector3 direction;
	direction.x = cosf(degreesToRadians(m_yaw)) * cosf(degreesToRadians(m_pitch));
	direction.y = sinf(degreesToRadians(m_pitch));
	direction.z = sinf(degreesToRadians(m_yaw)) * cosf(degreesToRadians(m_pitch));

	m_target = direction.normalize();
	


}

void Camera::Update()
{

	m_viewMatrix.LookAt(m_position,  m_position + m_target, m_up);
	m_projectionMatrix.Perspective(m_fov, m_width, m_height, m_minZ, m_maxZ);


	bIsDirty = false;

}




