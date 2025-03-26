#pragma once

#include "MathObjects.h"

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

	void RotateY(float angle, MatrixCollection& WVP) {                                           

		angle = degreesToRadians(angle);

		Matrix4 rotation;
		rotation.RotateY(angle);

		m_viewMatrix = rotation * m_viewMatrix;

		WVP.view = getViewMatrix();

		WVP.view.Transpose();
		WVP.world.Transpose();

	};

	void RotateZ(float angle) {

		angle = degreesToRadians(angle);

		Matrix4 rotation;
		rotation.RotateZ(angle);

		m_viewMatrix = rotation * m_viewMatrix;

	};

	void update(float deltaTime);

	Matrix4 getViewMatrix() const { return m_viewMatrix; }
	Matrix4 getProjectionMatrix() const { return m_projectionMatrix; }

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

	bool m_dirUp			= false;
	bool m_dirRight		= false;
	bool m_dirLeft		= false;
	bool m_dirDown		= false;
	bool m_dirForward	= false;
	bool m_dirBack		= false;
};

