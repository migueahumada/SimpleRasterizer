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


void Camera::Move(const Vector3& vector)
{
	m_viewMatrix.Translate(vector);
}

void Camera::Move(const Vector3& vector, float speed)
{
	m_viewMatrix.Translate(vector, speed);
}

void Camera::Move(const Vector3& vector, float speed, MatrixCollection& WVP)
{
	m_viewMatrix.Translate(vector, speed);

	WVP.world.Identity();
	WVP.view = getViewMatrix();

	WVP.view.Transpose();
	WVP.world.Transpose();
}

void Camera::MoveForwardVector(float speed, MatrixCollection& WVP)
{
	m_viewMatrix.Translate(GetForwardVector(), speed);

	WVP.world.Identity();
	WVP.view = getViewMatrix();

	WVP.view.Transpose();
	WVP.world.Transpose();
}

void Camera::MoveRightVector(float speed, MatrixCollection& WVP)
{
	m_viewMatrix.Translate(GetRightVector(), speed);

	WVP.world.Identity();
	WVP.view = getViewMatrix();

	WVP.view.Transpose();
	WVP.world.Transpose();
}

void Camera::MoveUpVector(float speed, MatrixCollection& WVP)
{
	m_viewMatrix.Translate(GetUpVector(), speed);

	WVP.world.Identity();
	WVP.view = getViewMatrix();

	WVP.view.Transpose();
	WVP.world.Transpose();
}



void Camera::MoveCamera(MatrixCollection& WVP, 
												float speed)
{
	if (m_dirForward) MoveForwardVector(m_speed * speed, WVP);
	if (m_dirBack)		MoveForwardVector(-m_speed * speed, WVP);
	if (m_dirLeft)		MoveRightVector(-m_speed * speed, WVP);
	if (m_dirRight)		MoveRightVector(m_speed * speed, WVP);
	if (m_dirUp)			MoveUpVector(m_speed * speed, WVP);
	if (m_dirDown)		MoveUpVector(-m_speed * speed, WVP);
}

void Camera::CheckMovement(CameraDirection::E direction)
{
	switch (direction)
	{
		case CameraDirection::UP:
			m_dirUp = true;
			break;
		case CameraDirection::DOWN:
			m_dirDown = true;
			break;
		case CameraDirection::RIGHT:
			m_dirRight = true;
			break;
		case CameraDirection::LEFT:
			m_dirLeft = true;
			break;
		case CameraDirection::FORWARD:
			m_dirForward = true;
			break;
		case CameraDirection::BACKWARDS:
			m_dirBack = true;
			break;
		default:
			break;
	}
}

void Camera::ResetMovement(CameraDirection::E direction)
{
	switch (direction)
	{
	case CameraDirection::UP:
		m_dirUp = false;
		break;
	case CameraDirection::DOWN:
		m_dirDown = false;
		break;
	case CameraDirection::RIGHT:
		m_dirRight = false;
		break;
	case CameraDirection::LEFT:
		m_dirLeft = false;
		break;
	case CameraDirection::FORWARD:
		m_dirForward = false;
		break;
	case CameraDirection::BACKWARDS:
		m_dirBack = false;
		break;
	default:
		break;
	}
}

void Camera::RotateX(float angle)
{
	angle = degreesToRadians(angle);

	Matrix4 rotation;
	rotation.RotateX(angle);

	m_viewMatrix = rotation * m_viewMatrix;

}

void Camera::RotateX(float angle, MatrixCollection& WVP)
{
	angle = degreesToRadians(angle);

	Matrix4 rotation;
	rotation.RotateX(angle);

	m_viewMatrix = rotation * m_viewMatrix;

	WVP.view = getViewMatrix();

	WVP.view.Transpose();
	WVP.world.Transpose();
}

void Camera::RotateY(float angle)
{
	angle = degreesToRadians(angle);

	Matrix4 rotation;
	rotation.RotateY(angle);

	m_viewMatrix = rotation * m_viewMatrix;
}

void Camera::RotateY(float angle, MatrixCollection& WVP)
{
	angle = degreesToRadians(angle);

	Matrix4 rotation;
	rotation.RotateY(angle);

	m_viewMatrix = rotation * m_viewMatrix;

	WVP.view = getViewMatrix();

	WVP.view.Transpose();
	WVP.world.Transpose();
}

void Camera::RotateZ(float angle)
{
	angle = degreesToRadians(angle);

	Matrix4 rotation;
	rotation.RotateZ(angle);

	m_viewMatrix = rotation * m_viewMatrix;
}

void Camera::RotateZ(float angle, MatrixCollection& WVP)
{
	angle = degreesToRadians(angle);

	Matrix4 rotation;
	rotation.RotateZ(angle);

	m_viewMatrix = rotation * m_viewMatrix;

	WVP.view = getViewMatrix();

	WVP.view.Transpose();
	WVP.world.Transpose();

}



void Camera::RotateCamera(MatrixCollection& WVP, 
													float angle, 
													float yaw, 
													float pitch)
{

	float anglePitch = radiansToDegrees(-angle);
	float angleYaw = radiansToDegrees(angle);

	//RotateZ(pitch * radiansToDegrees(0), WVP);
	RotateY(pitch * anglePitch, WVP);
	RotateX(yaw * angleYaw, WVP);
	
	
}

void Camera::Update(float dt)
{

}




