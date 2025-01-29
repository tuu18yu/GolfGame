#include "stdafx.h"

#include "Vec2.h"
#include <cmath>

const float PI = 3.141592f;

Vec2::Vec2()
	: x(0), y(0)
{
}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{
}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x || y != rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (const float val) const
{
	return Vec2(x / val, y / val);
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x * val, y * val);
}

void Vec2::operator += (const Vec2& rhs)
{
	x = x + rhs.x;
	y = y + rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x = x - rhs.x;
	y = y - rhs.y;
}

void Vec2::operator *= (const float val)
{
	x = x * val;
	y = y * val;
}

void Vec2::operator /= (const float val)
{
	x = x / val;
	y = y / val;
}

float Vec2::getLength() const
{
	return (float) sqrt(pow(x, 2.0) + pow(y, 2.0));
}

Vec2 Vec2::normalize() const
{
	float L = getLength();
	float x_normal = x / L;
	float y_normal = y / L;

	return Vec2(x_normal, y_normal);
}

float Vec2::getAngle() const
{
	return std::atan2(y, x) * (180 / PI);
}

float Vec2::getAngle(const Vec2& vec) const
{
	// arc cos(unit v_1 \dot unit v_2)
	float normalized_dot = normalize().dot_product(vec.normalize());
	return std::acosf(normalized_dot) * (180 / PI);
}

float Vec2::dot_product(const Vec2& vec) const
{
	return (x * vec.x) + (y * vec.y);
}

float Vec2::cross_product(const Vec2& vec) const
{
	return (x * vec.y) - (vec.x * y);
}

std::vector<Vec2> Vec2::interpolateY(const Vec2& vec) const
{
	std::vector<Vec2> filled_values;
	int x_min = static_cast<int>(round(std::min(x, vec.x)));
	int x_max = static_cast<int>(round(std::max(x, vec.x)));

	for (int xi = x_min; xi < x_max; xi++)
	{
		float yi = y + (((float) xi - x) * (vec.y - y) / (vec.x - x));
		filled_values.push_back({ (float) xi, yi });

	}

	return filled_values;
}

std::vector<Vec2> Vec2::interpolateX(const Vec2& vec) const
{
	std::vector<Vec2> filled_values;

	int y_min = static_cast<int>(round(std::min(y, vec.y)));
	int y_max = static_cast<int>(round(std::max(y, vec.y)));
	
	if (y_min == y_max) { return filled_values; }

	for (int yi = y_min; yi < y_max; yi++)
	{
		float xi = x + (((float)yi - y) * (vec.x - x) / (vec.y - y));
		filled_values.push_back({ xi, (float) yi });

	}

	return filled_values;
}