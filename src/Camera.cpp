#include "Camera.h"
#include <iostream>
#include "Renderer.h"

#define WIDTH 1280
#define HEIGHT 720

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

void Camera::UpdateAudioListener()
{
	m_Listener.OrientTop.x = m_up.x;
	m_Listener.OrientTop.y = m_up.y;
	m_Listener.OrientTop.z = m_up.z;

	m_Listener.OrientFront.x = -m_forward.x;
	m_Listener.OrientFront.y = -m_forward.y;
	m_Listener.OrientFront.z = -m_forward.z;

	m_Listener.Position.x = m_position.x;
	m_Listener.Position.y = m_position.y;
	m_Listener.Position.z = m_position.z;

	m_Listener.Velocity.x = m_velocity.x;
	m_Listener.Velocity.y = m_velocity.y;
	m_Listener.Velocity.z = m_velocity.z;
}

void Camera::SelectObjectOnScreen(float mouseX, float mouseY)
{

	// Clip Space Coords

	float clipX = ((mouseX / WIDTH) * 2.0f) - 1.0f;
	float clipY = ((mouseY / HEIGHT) * 2.0f) - 1.0f;

	// Se hace el punto de origen en coordenadas de clip.
	m_rayOrigin.x = clipX;
	m_rayOrigin.y = -clipY;
	m_rayOrigin.z = 0.0f; // profunidad 0 del frustrum, el near.
	m_rayOrigin.w = 1.0f;


	// Se hace el punto de objetivo en coordenadas de clip.
	m_rayTarget.x = clipX;
	m_rayTarget.y = -clipY;
	m_rayTarget.z = 1.0f; // profunidad de 1.0f, el far.
	m_rayTarget.w = 1.0f;

	// Calculamos la inversa de la vista proyección

	Matrix4 viewProjection = g_renderer().GetWVP().view *
													 g_renderer().GetWVP().projection;
	Matrix4 invViewProjection = viewProjection.Inverse();

	m_rayTarget = invViewProjection * m_rayTarget;
	m_rayOrigin = invViewProjection * m_rayOrigin;

	//Se proyecta
	m_rayOrigin.x = m_rayOrigin.x / m_rayOrigin.w;
	m_rayOrigin.y = m_rayOrigin.y / m_rayOrigin.w;
	m_rayOrigin.z = m_rayOrigin.z / m_rayOrigin.w;

	// Se proyecta
	m_rayTarget.x = m_rayTarget.x / m_rayTarget.w;
	m_rayTarget.y = m_rayTarget.y / m_rayTarget.w;
	m_rayTarget.z = m_rayTarget.z / m_rayTarget.w;
	
	m_rayDir = (m_rayTarget - m_rayOrigin).normalize();

	/*
	* ######################################
	*	############### TASK #################
	*	######################################
	*/
	//PASE CON UN PLANO EN LA PANTALLA PARA DIBUJAR CADA PIXEL DE LA PANTALLA
	//OBTENIENDO EL ESPACIO DEL RAYO Y PINTEN PIXEL SI HAY UN
	//CUANDO PASE EL PIXEL SHADER SE DIBUJE NE LA PANTALLA.
	//PONER LA GEOMETRÍA DE UN MODELO

	// SI EL PIXEL HACE EL HIT QUE SE DIBUJE ESE PIXEL EN LA PANTALLA SI NO, NO.

	/*
		sEA POR PIXEL Y DIBJUE TODO LO QUE SEA QUE CHOQUE.
		hÁGANLO EN SOFTWARE.
	*/

	// REIVSAR PIXEL POR PIXEL Y EN CAD APIXEL TIRAR DE LA PANTALLA, 
	// SI EL RAYO CHOCA CON LA GEOMETRÍA PONLO EN COLOR, SINO PONLO EN NEGRO.

	

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




