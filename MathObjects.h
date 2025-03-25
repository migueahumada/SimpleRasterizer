#pragma once

#define M_PI       3.14159265358979323846   // pi
#include <cmath>

using std::sqrtf;

inline float clamp(float value, float min, float max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}


/*
*	Para representar color necesitaremos una estructura que
*	guarde lo que llevaría un color , o sea RGBA.
*	Se guarda en char por que el tamaño es de 1 byte.
*	Es unsigned para que tome valores de 0 a 255
*/

struct Color { // 4 bytes
	unsigned char r; //1 byte
	unsigned char g; //1 byte
	unsigned char b; //1 byte
	unsigned char a; //1 byte
};

// dgrees = radians * 180/PI
static float radiansToDegrees(float angleInRadians) 
{
	return angleInRadians * (180.0f / M_PI);
}

static float degreesToRadians(float angleInDegrees) 
{
	return (angleInDegrees * M_PI) / 180.0f;
}



struct FloatColor {
	FloatColor() = default;
	FloatColor(float _r, float _g, float _b, float _a) :
		r(_r), g(_g), b(_b), a(_a) {}
	FloatColor(const Color& color) :
		r(color.r / 255.0f),
		g(color.g / 255.0f),
		b(color.b / 255.0f),
		a(color.a / 255.0f) {}

	Color toColor() {

		return Color{
			static_cast<unsigned char>(r * 255.0f),
			static_cast<unsigned char>(g * 255.0f),
			static_cast<unsigned char>(b * 255.0f),
			static_cast<unsigned char>(a * 255.0f)
		};
	}

	FloatColor operator*(const FloatColor& color) {
		return FloatColor(r * color.r, g * color.g, b * color.b, a * color.a);
	}

	FloatColor operator*(float escalar) {
		return FloatColor(r * escalar, g * escalar, b * escalar, a * escalar);
	}

	FloatColor operator+(const FloatColor& color) {
		return FloatColor(r + color.r, g + color.g, b + color.b, a + color.a);
	}

	FloatColor operator+(float escalar) {
		return FloatColor(r + escalar, g + escalar, b + escalar, a + escalar);
	}

	FloatColor operator-(const FloatColor& color) {
		return FloatColor(r - color.r, g - color.g, b - color.b, a - color.a);
	}

	FloatColor operator/(float escalar) {
		return FloatColor(r / escalar, g / escalar, b / escalar, a / escalar);
	}

	float toGrey() 
	{
		return (r * 0.299f) + (g * 0.587f) + (b * 0.114f);
	}

	FloatColor& saturate() {
		r = clamp(r, 0.0f, 1.0f);
		g = clamp(g, 0.0f, 1.0f);
		b = clamp(b, 0.0f, 1.0f);
		a = clamp(a, 0.0f, 1.0f);
		return *this;
	}

	float r;
	float g;
	float b;
	float a;
};


struct Vector3 
{
	float x;
	float y;
	float z;

	Vector3() = default;
	Vector3(float _x, float _y = 0.0f, float _z = 0.0f) : 
		x(_x), 
		y(_y), 
		z(_z) 
	{}

	Vector3 operator+(const Vector3& v) const {
		return { x + v.x, y + v.y, z + v.z };
	}

	Vector3 operator-(const Vector3& v) const {
		return { x - v.x, y - v.y, z - v.z };
	}

	Vector3 operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	Vector3 operator/(float scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}

	Vector3 operator-() const {
		return { -x , -y , -z };
	}

	Vector3 cross(const Vector3& v) const {
		return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
	}

	Vector3 operator^(const Vector3& v) const {
		return cross(v);
	}

	bool operator==(const Vector3& v) const {
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator !=(const Vector3& v) const {
		return !(*this == v);
	}

	float size() const {
		return sqrtf(x * x + y * y + z * z);
	}

	Vector3 normalize() const {
		float invLength = 1.0f / size();
		//float length = sqrt(x * x + y * y + z * z);
		return{ x * invLength,
				y * invLength,
				z * invLength };
	}

	float dot(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	float operator|(const Vector3& v) const {
		return dot(v);
	}

};

struct Matrix4
{
	Matrix4() = default;

	void Identity() 
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (i == j)
				{
					m[i][j] = 1.0f;
					continue;
				}
				
				m[i][j] = 0.0f;
				
			}
		}
	}
	//Es un sistema nuevo de coordenadas ltieralmente!!!
	void LookAt(const Vector3& eyePos, const Vector3& targetPos, const Vector3& upDir) 
	{
		const Vector3 ZAxis = (eyePos - targetPos).normalize();
		const Vector3 XAxis = upDir.cross(ZAxis).normalize();
		const Vector3 YAxis = ZAxis.cross(XAxis);

		m[0][0] = XAxis.x;
		m[1][0] = XAxis.y;
		m[2][0] = XAxis.z;

		m[0][1] = YAxis.x;
		m[1][1] = YAxis.y;
		m[2][1] = YAxis.z;

		m[0][2] = ZAxis.x;
		m[1][2] = ZAxis.y;
		m[2][2] = ZAxis.z;

		m[0][3] = 0.0f;
		m[1][3] = 0.0f;
		m[2][3] = 0.0f;

		Vector3 eyeNeg = -eyePos;

		m[3][0] = eyePos | XAxis;
		m[3][1] = eyePos | YAxis;
		m[3][2] = eyePos | ZAxis;
		m[3][3] = 1.0f;
	}

	//Proyección puede no ser una perspectiva
	//Usamos dos matrices de proyección diferentes.
	// Uno es usar matrices de perspectiva
	// Uno es perspecttiva ortográfica -> Es cuadrada, donde no queremos que haya deformación.

	//Se escala según la profundidad.
	void Perspective(float halfFOV, float widthScreen, float heightScreen, float MinZ, float MaxZ) 
	{
		float plane0[4] = { 1.0f / tanf(halfFOV), 0.0f, 0.0f, 0.0f };
		//Aspectratio-> dformación con respecto a la reosluciónd ela pantalla.
		float plane1[4] = { 0.0f, widthScreen / tanf(halfFOV) / heightScreen, 0.0f							,0.0f};
		float plane2[4] = { 0.0f, 0.0f										, MaxZ / (MaxZ - MinZ)			,1.0f };
		float plane3[4] = { 0.0f, 0.0f										,-MinZ * MaxZ / (MaxZ - MinZ)	,0.0f };
		
		m[0][0] = plane0[0];
		m[0][1] = plane0[1];
		m[0][2] = plane0[2];
		m[0][3] = plane0[3];
		
		m[1][0] = plane1[0];
		m[1][1] = plane1[1];
		m[1][2] = plane1[2];
		m[1][3] = plane1[3];

		m[2][0] = plane2[0];
		m[2][1] = plane2[1];
		m[2][2] = plane2[2];
		m[2][3] = plane2[3];

		m[3][0] = plane3[0];
		m[3][1] = plane3[1];
		m[3][2] = plane3[2];
		m[3][3] = plane3[3];
	
	
	}
	void Transpose() 
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = i + 1; j < 4; j++)
			{
				float temp = m[i][j];
				m[i][j] = m[j][i];
				m[j][i] = temp;
			}
		}
	}
	void Translate(const Vector3& translation) {
		m[3][0] += translation.x;
		m[3][1] += translation.y;
		m[3][2] += translation.z;
	}
	void Translate(const Vector3& translation, float deltaTime, float speed) {
		m[3][0] += translation.x * deltaTime * speed;
		m[3][1] += translation.y * deltaTime * speed;
		m[3][2] += translation.z * deltaTime * speed;
	}

	Vector3 TansformPosition(const Vector3& v) const 
	{
		return Vector3{
			v.x * m[0][0] + v.y * m[1][0] + v.x * m[2][0] + m[3][0] * 1.0f,
			v.x * m[0][1] + v.y * m[1][1] + v.x * m[2][1] + m[3][1] * 1.0f,
			v.x * m[0][2] + v.y * m[1][2] + v.x * m[2][2] + m[3][2] * 1.0f};
	}

	Vector3 TansformDirection(const Vector3& v) const
	{
		return Vector3{
			v.x * m[0][0] + v.y * m[1][0] + v.x * m[2][0] + m[3][0] * 0.0f,
			v.x * m[0][1] + v.y * m[1][1] + v.x * m[2][1] + m[3][1] * 0.0f,
			v.x * m[0][2] + v.y * m[1][2] + v.x * m[2][2] + m[3][2] * 0.0f };
	}

	void RotateY(float angle) {
		Identity();
		m[0][0] =  cos(angle);
		m[0][2] =  sin(angle);
		m[2][0] = -sin(angle);
		m[2][2] =  cos(angle);
	}

	void RotateX(float angle) {
		Identity();
		m[1][1] = cos(angle);
		m[1][2] = sin(angle);
		m[2][1] = -sin(angle);
		m[2][2] = cos(angle);
	}

	void RotateZ(float angle) {
		Identity();

		m[0][0] = cos(angle);
		m[1][0] = -sin(angle);
		m[0][1] = sin(angle);
		m[1][1] = cos(angle);
		
	}

	void Scale(float scale) {
		Identity();
		m[0][0] = scale;
		m[1][1] = scale;
		m[2][2] = scale;
	}

	void Scale(float scaleX, float scaleY, float ScaleZ) {
		Identity();
		m[0][0] = scaleX;
		m[1][1] = scaleY;
		m[2][2] = ScaleZ;
	}

	Matrix4 operator*(const Matrix4& matrix) const {
		Matrix4 result;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.m[i][j] = m[i][0] * matrix.m[0][j] +
								 m[i][1] * matrix.m[1][j] +
								 m[i][2] * matrix.m[2][j] +
								 m[i][3] * matrix.m[3][j];
			}
		}
		return result;
	}

	float m[4][4];
};

struct Vertex {
	Vector3 position;
	Color color;
	float u, v;
};

struct Triangle {
	Vertex v1, v2, v3;
	Vector3 normal;
};

struct Camera 
{
	void SetLookAt(const Vector3& eyePos, const Vector3& targetPos, const Vector3& upDir) 
	{
		position = eyePos;
		target = targetPos;
		up = upDir;
		viewMatrix.LookAt(position, target, up);
	}

	void SetPerspective(float halfFOV, float widthScreen, float heightScreen, float MinZ, float MaxZ) {
		fov = halfFOV;
		width = widthScreen;
		height = heightScreen;
		minZ = MinZ;
		maxZ = MaxZ;

		projectionMatrix.Perspective(fov,width,height,minZ,maxZ);
	}

	Vector3 GetForwardVector() const{
		return (position - target).normalize();
	}

	Vector3 GetRightVector() const{
		return (up^ GetForwardVector()).normalize();
	}

	Vector3 GetUpVector() const {
		return (GetForwardVector()^ GetRightVector()).normalize();
	}


	void Move(const Vector3& vector) {
		viewMatrix.Translate(vector);
	}

	void Move(const Vector3& vector,float deltaTime, float speed) {
		viewMatrix.Translate(vector,deltaTime,speed);
	}

	//Angle in degrees
	void RotateX(float angle) {
		
		angle = degreesToRadians(angle);
		
		Matrix4 rotation;
		rotation.RotateX(angle);

		viewMatrix = rotation * viewMatrix;
		
	};

	void RotateY(float angle) {

		angle = degreesToRadians(angle);

		Matrix4 rotation;
		rotation.RotateY(angle);

		viewMatrix = rotation * viewMatrix;

	};

	void RotateZ(float angle) {

		angle = degreesToRadians(angle);

		Matrix4 rotation;
		rotation.RotateZ(angle);

		viewMatrix = rotation * viewMatrix;

	};

	Matrix4 getViewMatrix() const { return viewMatrix; }
	Matrix4 getProjectionMatrix() const { return projectionMatrix; }

private:
	Vector3 position;
	Vector3 target;
	Vector3 up;

	float fov;
	float height;
	float width;
	float minZ;
	float maxZ;
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
};
