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
		return (m_target - m_position).normalize();
	}


	inline Vector3 GetRightVector() const {
		return (m_up ^ GetForwardVector()).normalize();
	}

	inline Vector3 GetUpVector() const {
		return (-GetForwardVector() ^ GetRightVector()).normalize();
	}

	//THIS IS THE MOVE FUNCTION FOR UPDATE
	void Move(const Vector3& vector);						


	void Rotate(float newYaw, float newPitch);


	inline Matrix4 getViewMatrix() 
	{ 
		if (bIsDirty)
		{
			Update();
		}
		return m_viewMatrix; 
	}
	inline Matrix4 getProjectionMatrix() 
	{ 
		if (bIsDirty)
		{
			Update();
		}
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

	inline float getRotSpeed() const
	{
		return m_rotSpeed;
	}
	inline X3DAUDIO_LISTENER getCameraListener() const{
		return m_cameraListener;
	}

	inline void setCameraListener(X3DAUDIO_LISTENER& listener)
	{
		memcpy(&m_cameraListener, &listener, sizeof(X3DAUDIO_LISTENER));
	}

	inline Vector3 GetViewDir() const{
		return (m_target - m_position).normalize();
	}

	inline float getYaw() const{
		return m_yaw;
	}

	inline float getPitch() const{
		return m_pitch;
	}

	inline float getRoll() const{
		return m_roll;
	}

	inline void setYaw(float newYaw){
		m_yaw = newYaw;
	}

	inline void setPitch(float newPitch) {
		m_yaw = newPitch;
	}

	inline void setRoll(float newRoll) {
		m_yaw = newRoll;
	}

	void Update();

private:

	/*	Para crear una cámara necesito :
	*   - VectorPosición
	*/

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

	float m_speed = 10.0f;
	float m_rotSpeed = 0.1f;

	float m_yaw;
	float m_pitch;
	float m_roll;

	bool bIsDirty = false;

	X3DAUDIO_LISTENER m_cameraListener = {};
};

