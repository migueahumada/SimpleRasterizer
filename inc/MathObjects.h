#pragma once

#define M_PI       3.14159265358979323846   // pi
#include <cmath>
#include <cassert>
#include "HelperMacros.h"
#include <stdexcept>
using std::sqrtf;


inline float clamp(float value, float min, float max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

struct Matrix4;

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
	return static_cast<float>(angleInRadians * (180.0f / M_PI));
}

static float degreesToRadians(float angleInDegrees) 
{
	return static_cast<float>((angleInDegrees * M_PI) / 180.0f);
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

struct Vector2
{

	Vector2() = default;
	Vector2(float _x, float _y = 0.0f) :
		x(_x),
		y(_y){}
	
	float x;
	float y;
};



struct Vector3 
{
	float x;
	float y;
	float z;

	Vector3() = default;
	Vector3(float _x , float _y = 0.0f, float _z = 0.0f) : 
		x(_x), 
		y(_y), 
		z(_z) 
	{}

	inline Vector3 operator+(const Vector3& v) const {
		return { x + v.x, y + v.y, z + v.z };
	}

	inline Vector3& operator+=(const Vector3& v) {
		x += v.x; 
		y += v.y;
		z += v.z; 
		return *this;
	}

	inline Vector3& operator-=(const Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	inline Vector3 operator-(const Vector3& v) const {
		return { x - v.x, y - v.y, z - v.z };
	}

	inline Vector3 operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	inline Vector3 operator/(float scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}

	inline Vector3 operator-() const {
		return { -x , -y , -z };
	}

	inline Vector3 cross(const Vector3& v) const {
		return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x };
	}

	inline Vector3 operator^(const Vector3& v) const {
		return cross(v);
	}

	inline bool operator==(const Vector3& v) const {
		return x == v.x && y == v.y && z == v.z;
	}

	inline bool operator !=(const Vector3& v) const {
		return !(*this == v);
	}

	inline float size() const {
		return sqrtf(x * x + y * y + z * z);
	}

	Vector3 normalize() const {
		float invLength = 1.0f / size();
		//float length = sqrt(x * x + y * y + z * z);
		return{ x * invLength,
				y * invLength,
				z * invLength };
	}

	inline float dot(const Vector3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	inline float operator|(const Vector3& v) const {
		return dot(v);
	}

	

	

};

struct Vector4
{

	Vector4() = default;
	Vector4(float _x = 0.0f, 
					float _y = 0.0f, 
					float _z = 0.0f, 
					float _w = 0.0f)
					: x(_x), 
						y(_y), 
						z(_z), 
						w(_w){}

	inline Vector4 operator+(const Vector4& rhs) const
	{
		return Vector4{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
	}

	inline Vector4 operator-(const Vector4& rhs) const
	{
		return Vector4{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
	}

	inline Vector4 operator*(float scale) const
	{
		return Vector4{ x * scale, y * scale, z * scale, w * scale };
	}

	inline float size() const
	{
		return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
	}

	inline Vector4 normalize() const
	{
		float invLength = 1.0f / size();

		return Vector4
		{ 
			x * invLength,
			y * invLength,
			z * invLength,
			w * invLength 
		};
	}

	float x;
	float y;
	float z;
	float w;

};

struct Matrix2{
	Matrix2()
	{
		Identity();
	}

	void Identity()
	{
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 2; ++j)
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

	float Determinant()
	{
		/*    0  1
			 0 [x][x]
			 1 [x][x]
		*/

		return m[0][0] * m[1][0] - m[1][0] * m[1][1];
	}

	float m[2][2];
};

template <typename T, uint32 N>
class Matrix {
public:
	Matrix()
	{
		static_assert(std::is_integral_v<T> == true);
		Identity();
	}

	void Identity()
	{
		for (T i = 0; i < N; ++i)
		{
			for (T j = 0; j < N; ++j)
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

	float m[N][N];
};


struct Matrix3 
{
	Matrix3()
	{
		Identity();
	}

	void Identity()
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
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

	/*
				0  1  2
			0[x][x][x]
			1[x][x][x]
			2[x][x][x]
	*/
	float Determinant()
	{
		return	(m[0][0] * m[1][1] * m[2][2]) + 
						(m[0][1] * m[1][2] * m[2][0]) + 
						(m[0][2] * m[1][0] * m[2][1]) - 
						(m[0][2] * m[1][1] * m[2][0]) -
						(m[0][1] * m[1][0] * m[2][2]) -
						(m[0][0] * m[1][2] * m[2][1]);
	}

	void Transpose()
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = i + 1; j < 3; ++j)
			{
				float temp = m[i][j];
				m[i][j] = m[j][i];
				m[j][i] = temp;
			}
		}
	}

	Matrix3 operator*(float scalar) 
	{
		Matrix3 result;
		for (size_t i = 0; i < 3; ++i)
		{
			for (size_t j = 0; j < 3; ++j)
			{
				result.m[i][j] = m[i][j] * scalar;
			}
		}

		return result;
	}

	/*
					0  1  2
			 0 [x][x][x]
			 1 [x][x][x]
			 2 [x][x][x]

			 [1][1][2]
			 [1][3][4]
			 [0][2][5]

			 [c1][c1][c2]
			 [c1][c3][c4]
			 [c0][c2][c5]
		*/
	Matrix3 Inverse()
	{

		float det = Determinant();

		//Los cofactores de la matriz se calculan como normal
		Matrix3 mat;
		mat.m[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
		mat.m[0][1] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]);
		mat.m[0][2] = m[1][0] * m[2][1] - m[1][1] * m[2][0];

		mat.m[1][0] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]);
		mat.m[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
		mat.m[1][2] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]);

		mat.m[2][0] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
		mat.m[2][1] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]);
		mat.m[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];

		//Se transpone
		mat.Transpose();

		//Se multiplica toda la matriz por el recíproco del valor de la determinante.
		Matrix3 invMat = mat * (1.0f/det);

		return invMat;

	}
	
	float m[3][3];
};

struct __declspec(align(16)) Matrix4
{
	Matrix4() {
		Identity();
	}

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

	Matrix4 operator*(float scalar)
	{
		Matrix4 result;
		for (size_t i = 0; i < 4; ++i)
		{
			for (size_t j = 0; j < 4; ++j)
			{
				result.m[i][j] = m[i][j] * scalar;
			}
		}

		return result;
	}

	Matrix3 GetSubMatrix(uint8 fila, uint8 columna)
	{
		Matrix3 subMatrix;

		uint8 r = 0;
		for (uint8 i = 0; i < 4; ++i)
		{
			if (i == fila)
			{
				continue;
			}

			int c = 0;
			for (uint8 j = 0; j < 4; ++j)
			{
				if (j == columna)
				{
					continue;
				}

				subMatrix.m[r][c] = m[i][j];
				++c;
			}
			++r;
		}
			
		return subMatrix;
	}

	Matrix4 Inverse() 
	{
		Matrix4 inv;

		float det = Determinant();

		/*
					0  1  2  3
			 0 [x][x][x][x]
			 1 [x][0][x][x]
			 2 [x][x][x][x]
			 3 [x][x][x][x]


					0  1  2
			 0 [x][x][x]
			 1 [x][x][x]
			 2 [x][x][x]
		*/

		Matrix3 m00, m01, m02, m03,
						m10, m11, m12, m13,
						m20, m21, m22, m23,
						m30, m31, m32, m33;

		m00 = GetSubMatrix(0, 0); m01 = GetSubMatrix(0, 1); m02 = GetSubMatrix(0, 2); m03 = GetSubMatrix(0, 3);
		m10 = GetSubMatrix(1, 0); m11 = GetSubMatrix(1, 1); m12 = GetSubMatrix(1, 2); m13 = GetSubMatrix(1, 3);
		m20 = GetSubMatrix(2, 0); m21 = GetSubMatrix(2, 1); m22 = GetSubMatrix(2, 2); m23 = GetSubMatrix(2, 3);
		m30 = GetSubMatrix(3, 0); m31 = GetSubMatrix(3, 1); m32 = GetSubMatrix(3, 2); m33 = GetSubMatrix(3, 3);

		float c00, c01, c02, c03,
					c10, c11, c12, c13,
					c20, c21, c22, c23,
					c30, c31, c32, c33;

		c00 = m00.Determinant(); c01 = m01.Determinant(); c02 = m02.Determinant(); c03 = m03.Determinant();
		c10 = m10.Determinant(); c11 = m11.Determinant(); c12 = m12.Determinant(); c13 = m13.Determinant();
		c20 = m20.Determinant(); c21 = m21.Determinant(); c22 = m22.Determinant(); c23 = m23.Determinant();
		c30 = m30.Determinant(); c31 = m31.Determinant(); c32 = m32.Determinant(); c33 = m33.Determinant();

		inv.m[0][0] =  c00; inv.m[0][1] = -c01; inv.m[0][2] =  c02; inv.m[0][3] = -c03;
		inv.m[1][0] = -c10; inv.m[1][1] =  c11; inv.m[1][2] = -c12; inv.m[1][3] =  c13;
		inv.m[2][0] =  c20; inv.m[2][1] = -c21; inv.m[2][2] =  c22; inv.m[2][3] = -c23;
		inv.m[3][0] = -c30; inv.m[3][1] =  c31; inv.m[3][2] = -c32; inv.m[3][3] =  c33;

		inv.Transpose();

		//Se multiplica toda la matriz por el recíproco del valor de la determinante.
		Matrix4 invMat = inv * (1.0f / det);
		
		return invMat;
	}

	float Determinant() const
	{
		/*	  
		      0  1  2  3
			 0 [x][x][x][x]
			 1 [x][x][x][x]
			 2 [x][x][x][x]
			 3 [x][x][x][x]

			    0  1  2
			 0 [x][x][x]
			 1 [x][x][x]
			 2 [x][x][x]
			 
		*/

		float e1 = m[0][0];
		float e2 = m[0][1];
		float e3 = m[0][2];
		float e4 = m[0][3];

		Matrix3 m1;
		m1.m[0][0] = m[1][1];
		m1.m[0][1] = m[1][2];
		m1.m[0][2] = m[1][3];

		m1.m[1][0] = m[2][1];
		m1.m[1][1] = m[2][2];
		m1.m[1][2] = m[2][3];

		m1.m[2][0] = m[3][1];
		m1.m[2][1] = m[3][2];
		m1.m[2][2] = m[3][3];

		Matrix3 m2;
		m2.m[0][0] = m[1][0];
		m2.m[0][1] = m[1][2];
		m2.m[0][2] = m[1][3];

		m2.m[1][0] = m[2][0];
		m2.m[1][1] = m[2][2];
		m2.m[1][2] = m[2][3];

		m2.m[2][0] = m[3][0];
		m2.m[2][1] = m[3][2];
		m2.m[2][2] = m[3][3];

		Matrix3 m3;
		m3.m[0][0] = m[1][0];
		m3.m[0][1] = m[1][1];
		m3.m[0][2] = m[1][3];

		m3.m[1][0] = m[2][0];
		m3.m[1][1] = m[2][1];
		m3.m[1][2] = m[2][3];

		m3.m[2][0] = m[3][0];
		m3.m[2][1] = m[3][1];
		m3.m[2][2] = m[3][3];

		Matrix3 m4;
		m4.m[0][0] = m[1][0];
		m4.m[0][1] = m[1][1];
		m4.m[0][2] = m[1][2];

		m4.m[1][0] = m[2][0];
		m4.m[1][1] = m[2][1];
		m4.m[1][2] = m[2][2];

		m4.m[2][0] = m[3][0];
		m4.m[2][1] = m[3][1];
		m4.m[2][2] = m[3][2];

		
		return (e1 * m1.Determinant()) - 
					 (e2 * m2.Determinant()) +
					 (e3 * m3.Determinant()) - 
					 (e4 * m4.Determinant());
		
	}


	//Es un sistema nuevo de coordenadas ltieralmente!!!
	void LookAt(const Vector3& eyePos, const Vector3& targetPos, const Vector3& upDir) 
	{
		const Vector3 ZAxis = (targetPos - eyePos).normalize();
		const Vector3 XAxis = upDir.cross(ZAxis).normalize();
		const Vector3 YAxis = ZAxis.cross(XAxis);

		/*
			[xx][yx][zx][wx]
			[yx][yy][yz][wy]
			[zx][yz][zz][wz]
			[0 ][0 ][0 ][1 ]
		*/

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

		m[3][0] = eyeNeg | XAxis;
		m[3][1] = eyeNeg | YAxis;
		m[3][2] = eyeNeg | ZAxis;
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
		float plane1[4] = { 0.0f, widthScreen / tanf(halfFOV) / heightScreen, 0.0f ,0.0f};
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
	
	void SetOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ)
	{
	
		Identity();
		m[0][0] = 2.0f / (right - left);
		m[1][1] = 2.0f / (top - bottom);
		m[2][2] = 1.0f / (farZ - nearZ);
		m[3][0] = -(right + left) / (right - left);
		m[3][1] = -(top + bottom) / (top - bottom);
		m[3][2] = -nearZ / (farZ - nearZ);
		m[3][3] = 1.0f;

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
		Identity();
		m[3][0] += translation.x;
		m[3][1] += translation.y;
		m[3][2] += translation.z;
	}
	void Translate(const Vector3& translation,float speed) {
		m[3][0] += translation.x * speed;
		m[3][1] += translation.y * speed;
		m[3][2] += translation.z * speed;
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

	void RotateX(float angle) {
		
		m[1][1] = cos(angle);
		m[1][2] = sin(angle);
		m[2][1] = -sin(angle);
		m[2][2] = cos(angle);
	}

	void RotateY(float angle) {
		
		m[0][0] =  cos(angle);
		m[0][2] = -sin(angle);
		m[2][0] = sin(angle);
		m[2][2] =  cos(angle);
	}


	void RotateZ(float angle) {
		
		
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

	static Vector3 VectorMultiplication(const Vector3& v, const Matrix4& m)
	{
		/*
											[][][][]
			  [][][][]  x		[][][][]
											[][][][]
											[][][][]
		*/
		float x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + 1.0f * m.m[3][0];
		float y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + 1.0f * m.m[3][1];
		float z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + 1.0f * m.m[3][2];
		float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + 1.0f * m.m[3][3];

		return Vector3{ x, y, z };
	}

	Vector3 operator*(const Vector3& v) const{
		return Vector3{
			v.x * m[0][0] + v.y * m[0][1] + v.z * m[0][2],
			v.x * m[1][0] + v.y * m[1][1] + v.z * m[1][2],
			v.x * m[2][0] + v.y * m[2][1] + v.z * m[2][2],
		};
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

//Para crearlo tiene que sermultiplo de 16


struct Quaternion
{
  Quaternion(float _w = 0.0f, float _x = 1.0f, float _y = 1.0f, float _z = 1.0f)
						: w (_w), v(Vector3{_x,_y,_z}) {}

	Quaternion(float _w = 0.0f, const Vector3& _v = Vector3{1.0f,1.0f,1.0f}) 
						: w(_w), v(_v) {}
	
	//ADDITION
	inline Quaternion operator+(const Quaternion& q)
	{
		return Quaternion(w + q.w, Vector3{ v.x + q.v.x,
																				v.y + q.v.y,
																				v.z + q.v.z });
	}

	//SUBTRACTION
	inline Quaternion operator-(const Quaternion& q)
	{
		return Quaternion(w - q.w, Vector3{ v.x - q.v.x,
																				v.y - q.v.y,
																				v.z - q.v.z });
	}

	//SCALING
	inline Quaternion operator*(float scalar)
	{
		return Quaternion(w * scalar, v * scalar);
	}
	
	//PRODUCT
	inline Quaternion operator*(const Quaternion& q)
	{
		float re = (w * q.w) - (v|q.v);
		Vector3 img = (q.v * w)+(v * q.w)+(v^q.v);

		return Quaternion(re,img);
	}

	//MAGNITUDE
	inline float size() const
	{
		return sqrtf(w*w + v.x*v.x + v.y*v.y + v.z*v.z);
	}

	//NORMALIZE
	inline Quaternion normalize() const
	{
		float length = 1.0f/ size();
		return Quaternion(w * length, v * length);
	}

	//DOT PRODUCT
	inline float dot(const Quaternion& q) const
	{
		return w * q.w + v.x * q.v.x + v.y * q.v.y + v.z * q.v.z;
	}

	inline float operator|(const Quaternion& q) const
	{
		return dot(q);
	}

	//INVERSE
	inline Quaternion inverse() const{
		return Quaternion(w,-v);
	}

	//IDENTITY
	inline Quaternion identity() const
	{
		return Quaternion(1.0f,v*0.0f);
	}

	Vector3 v;
	float w;

};

class Transform {

public:
	Transform(Vector3 _position = Vector3{0.0f,0.0f,0.0f}, 
						Vector3 _rotation = Vector3{ 0.0f,0.0f,0.0f }, 
						Vector3 _scale = Vector3{ 1.0f,1.0f,1.0f })
						: m_position(_position),m_rotation(_rotation), m_scale(_scale){}
	~Transform() = default;
	
	Matrix4 getMatrix() const
	{
		Matrix4 matrix;

		Matrix4 scale;
		scale.Scale(m_scale.x, m_scale.y, m_scale.z);

		Matrix4 rotationX;
		rotationX.RotateX(degreesToRadians(m_rotation.x));

		Matrix4 rotationY;
		rotationY.RotateY(degreesToRadians(m_rotation.y));
		
		Matrix4 rotationZ;
		rotationZ.RotateZ(degreesToRadians(m_rotation.z));
		
		Matrix4 translation;
		translation.Translate(m_position);
		
		matrix = scale * rotationZ * rotationY * rotationX  * translation;
		
		return matrix;
	}

	Vector3 getPosition() const{
		return m_position;
	}

	Vector3 getRotation() const {
		return m_rotation;
	}

	Vector3 getScale() const {
		return m_scale;
	}

	void setPosition(const Vector3& v)
	{
		m_position = v;
	}

	void setRotation(const Vector3& v)
	{
		m_rotation = v;
	}

	void setScale(const Vector3& v)
	{
		m_scale = v;
	}


private:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;
};

//Formula for rotation -> qpq^-1
//q = quaternion
//p = vector or a point in space
//q^-1 = inverse of the quaternion
//TODO: COMPLETAR ESTE PEDO
//Vector3 quatRotation(const Vector3& v, const Quaternion& q)
//{
//	q.normalize();
//	Quaternion qConjugate = q.inverse();
//}

/*
* A simple 4x4 matrix

 X   Y   Z   W
---------------
[1] [0] [0]	[0],
[0] [1] [0]	[0],
[0] [0] [1]	[0],
[0] [0] [0]	[1],

*/