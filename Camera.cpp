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

void Camera::SetOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ)
{
	m_projectionMatrix.SetOrthographic(left, right, bottom, top, nearZ, farZ);
}


void Camera::Move(const Vector3& direction)
{
  //se multiplica el vector de dirección por la velocidad en Z
	//Es como multiplicar el vector por un escalar, o sea escalar el vector a través del tiempo.
	//En cada eje se debe de multiplicar por su vector ya sea forwrd, up o right.
	m_position += m_forward * (direction.z * m_speed);
	m_position += m_right * (direction.x * m_speed);
	m_position += m_up * (direction.y * m_speed);

	//m_position += direction * m_speed;

	bIsDirty = true;
}

void Camera::Rotate(float newYaw, float newPitch)
{
	m_yaw += -newYaw;
	m_pitch += -newPitch;

	//Caluclar vector de dirección ya con yaw y pitch
	Vector3 forwardDir;
	forwardDir.x = cosf(degreesToRadians(m_yaw)) * cosf(degreesToRadians(m_pitch));
	forwardDir.y = sinf(degreesToRadians(m_pitch));
	forwardDir.z = sinf(degreesToRadians(m_yaw)) * cosf(degreesToRadians(m_pitch));

	//Se crea el vector forward y se updatea el right y el up de acuerdo al up del mundo.
	m_forward = forwardDir.normalize(); //Forward es vec de dirección
	m_right = (Vector3(0.0f,1.0f,0.0f) ^ m_forward).normalize();
	m_up = (m_forward^ m_right).normalize();

	

	bIsDirty = true;
}

void Camera::Update()
{
	
	m_viewMatrix.LookAt(m_position, m_forward + m_position, m_up);
	m_projectionMatrix.Perspective(m_fov, m_width, m_height, m_minZ, m_maxZ);

	bIsDirty = false;

}




