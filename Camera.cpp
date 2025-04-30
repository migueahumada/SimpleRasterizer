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
	m_target += direction * m_speed;

	bIsDirty = true;
}



//void Camera::Move(const Vector3& vector, float speed, MatrixCollection& WVP)
//{
//	m_viewMatrix.Translate(vector, speed);
//
//	WVP.world.Identity();
//	WVP.view = getViewMatrix();
//
//	WVP.view.Transpose();
//	WVP.world.Transpose();
//}

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


void Camera::Update()
{

	m_viewMatrix.LookAt(m_position, m_target, m_up);
	m_projectionMatrix.Perspective(m_fov, m_width, m_height, m_minZ, m_maxZ);

	bIsDirty = false;
	/*if (bIsDirty)
	{
		if (m_dirForward) { Move(GetForwardVector() * dt);	};
		if (m_dirBack)		{ Move(-GetForwardVector() * dt); bIsDirty = false; }
		if (m_dirRight)		{ Move(GetRightVector() * dt);		bIsDirty = false; }
		if (m_dirLeft)		{ Move(-GetRightVector() * dt);		bIsDirty = false; }
		if (m_dirUp)			{ Move(GetUpVector() * dt);				bIsDirty = false; }
		if (m_dirDown)		{ Move(-GetUpVector() * dt);			bIsDirty = false; }
	}*/
}




