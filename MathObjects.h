#pragma once
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


struct Vector3 {
	float x;
	float y;
	float z;

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

struct Vertex {
	Vector3 position;
	Color color;
	float u, v;
};

struct Triangle {
	Vertex v1, v2, v3;
};
