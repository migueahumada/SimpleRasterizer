#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "MathObjects.h"
#include <stdio.h>
#include <x3daudio.h>
#include <SDL3/SDL.h>

namespace CameraDirection{
	enum E {
		UP				= 0,
		DOWN			= 1,
		RIGHT			= 2,
		LEFT			= 3,
		FORWARD		= 4,
		BACKWARDS = 5
	};
}

class Camera
{
	
public:

	Camera() = default;
	~Camera() = default;

	void SetLookAt(	const Vector3& eyePos, 
									const Vector3& targetPos, 
									const Vector3& upDir);

	void SetPerspective(float halfFOV, 
											float widthScreen, 
											float heightScreen, 
											float MinZ, 
											float MaxZ);

	inline Vector3 GetForwardVector() const {
		return (m_position - m_target).normalize();
	}

	inline Vector3 GetRightVector() const {
		return (m_up ^ GetForwardVector()).normalize();
	}

	inline Vector3 GetUpVector() const {
		return (-GetForwardVector() ^ GetRightVector()).normalize();
	}

	//Moves one unit in the direction the vector is pointing to
	void Move(const Vector3& vector);

	//Moves one unit in the direction the vector is pointing to, 
	//but with speed
	void Move(const Vector3& vector, float speed);

	//Moves one unit in the direction the vector is pointing to, 
	//but with speed encapsulating the WVP
	void Move(const Vector3& vector, 
						float speed, 
						MatrixCollection& WVP);

	void MoveForwardVector(	float speed, 
													MatrixCollection& WVP);

	void MoveRightVector(	float speed, 
												MatrixCollection& WVP);

	void MoveUpVector(float speed, 
										MatrixCollection& WVP);
							
	void MoveCamera(MatrixCollection& WVP, 
									float speed);
	
	void CheckMovement(CameraDirection::E direction);

	void ResetMovement(CameraDirection::E direction);

	//Angle in degrees
	void RotateX(float angle);

	void RotateX(float angle, MatrixCollection& WVP);

	void RotateY(float angle);

	void RotateY(float angle, MatrixCollection& WVP);

	void RotateZ(float angle);
	
	void RotateZ(float angle, MatrixCollection& WVP);

	inline float getYaw() const{
		return atan2(GetForwardVector().x, GetForwardVector().z);
	}

	inline float getPitch() const {
		return asinf(GetForwardVector().y);
	}

	inline float getRoll() const {
		return atan2(GetRightVector().y,GetUpVector().y);
	}

	void RotateCamera(MatrixCollection& WVP, 
										float angle, 
										float yaw, 
										float pitch);

	inline Matrix4 getViewMatrix() const 
	{ 
		return m_viewMatrix; 
	}
	inline Matrix4 getProjectionMatrix() const 
	{ 
		return m_projectionMatrix; 
	}
	inline Vector3 getEyePosition() const
	{ 
		return m_position;
	}

	inline float getVelocity() const
	{
		return m_speed;
	}

	inline X3DAUDIO_LISTENER getCameraListener() const{
		return m_cameraListener;
	}

	inline void setCameraListener(X3DAUDIO_LISTENER& listener)
	{
		memcpy(&m_cameraListener, &listener, sizeof(X3DAUDIO_LISTENER));
	}

	void Update(float dt);


private:

	Vector3 m_position;
	Vector3 m_target;
	Vector3 m_up;

	float m_fov;
	float m_height;
	float m_width;
	float m_minZ;
	float m_maxZ;
	
	Matrix4 m_viewMatrix;
	Matrix4 m_projectionMatrix;

	float m_speed = 1.0f;
	float m_rotSpeed = 1.0f;

	bool m_dirUp			= false;
	bool m_dirRight		= false;
	bool m_dirLeft		= false;
	bool m_dirDown		= false;
	bool m_dirForward	= false;
	bool m_dirBack		= false;

	X3DAUDIO_LISTENER m_cameraListener = {};
};

