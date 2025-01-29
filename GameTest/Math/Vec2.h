#pragma once
#include <vector>
class Vec2
{
public:

	float x = 0;
	float y = 0;

	Vec2();
	Vec2(float xin, float yin);

	bool operator == (const Vec2& rhs) const;
	bool operator != (const Vec2& rhs) const;

	Vec2 operator + (const Vec2& rhs) const;
	Vec2 operator - (const Vec2& rhs) const;
	Vec2 operator / (const float val) const;
	Vec2 operator * (const float val) const;

	void operator += (const Vec2& rhs);
	void operator -= (const Vec2& rhs);
	void operator *= (const float val);
	void operator /= (const float val);

	float getLength() const;
	Vec2 normalize() const;

	float getAngle() const; // angle between x-axis
	float getAngle(const Vec2& vec) const; // angle between given angle 

	float dot_product(const Vec2& vec) const;
	
	// There is no cross product in 2D, but we imagine the result of 2D
	// cross product is the vector pointing towards the screen
	// The function returns a float value for length of the "z" component
	// of the imaginary vector
	float cross_product(const Vec2& vec) const;

	// Given another postion on the line, return linear interpolated Vec2 between two points
	std::vector<Vec2> interpolateY(const Vec2& vec) const;
	std::vector<Vec2> interpolateX(const Vec2& vec) const;
};