#include "stdafx.h"

#include "Vec3.h"
#include <cmath>

#include <stdexcept>

#define PI 3.14159265

Vec3::Vec3()
	: x(0), y(0), z(0)
{
}

Vec3::Vec3(float xin, float yin, float zin)
	: x(xin), y(yin), z(zin)
{
}

float& Vec3::operator [] (int index)
{
	switch (index) 
	{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default:
			throw std::out_of_range("Index out of range for Vec3.");
	}
}

const float& Vec3::operator[](int index) const 
{
	switch (index) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default:
			throw std::out_of_range("Index out of range for vec3.");
	}
}

bool Vec3::operator == (const Vec3& rhs) const
{
	return (x == rhs.x && y == rhs.y && z == rhs.z);
}

bool Vec3::operator != (const Vec3& rhs) const
{
	return (x != rhs.x || y != rhs.y || z != rhs.z);
}

Vec3 Vec3::operator + (const Vec3& rhs) const
{
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3 Vec3::operator - (const Vec3& rhs) const
{
	return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3 Vec3::operator / (const float val) const
{
	return Vec3(x / val, y / val, z/val);
}

Vec3 Vec3::operator * (const float val) const
{
	return Vec3(x * val, y * val, z * val);
}

Vec3 Vec3::operator * (const Vec3& val) const
{
	return Vec3(x * val.x, y * val.y, z * val.z);
}

void Vec3::operator += (const Vec3& rhs)
{
	x = x + rhs.x;
	y = y + rhs.y;
	z = z + rhs.z;
}

void Vec3::operator -= (const Vec3& rhs)
{
	x = x - rhs.x;
	y = y - rhs.y;
	z = z - rhs.z;
}

void Vec3::operator *= (const float val)
{
	x = x * val;
	y = y * val;
	z = z * val;
}

void Vec3::operator /= (const float val)
{
	x = x / val;
	y = y / val;
	z = z / val;
}

float Vec3::getLength() const
{
	return sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::normalize() const
{
	float L = getLength();

	// if length is zero, it's zero
	if (L == 0.0f)
	{
		return Vec3(0.0f, 0.0f, 0.0f);
	}
	return Vec3(x / L, y / L, z / L);
}

float Vec3::dot_product(const Vec3& vec) const
{
	return (x * vec.x) + (y * vec.y) + (z * vec.z);
}

Vec3 Vec3::cross_product(const Vec3& vec) const
{
	float x_cross = y * vec.z - z * vec.y;
	float y_cross = z * vec.x - x * vec.z;
	float z_cross = x * vec.y - y * vec.x;

	return Vec3(x_cross, y_cross, z_cross);
}