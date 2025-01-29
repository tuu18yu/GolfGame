#pragma once

class Vec3
{
public:
	// x, y, z values are public as vector vaules are modified often
	float x = 0;
	float y = 0;
	float z = 0;

	Vec3();
	Vec3(float xin, float yin, float zin);

	float& operator[](int index);
	const float& operator[](int index) const;

	bool operator == (const Vec3& rhs) const;
	bool operator != (const Vec3& rhs) const;

	Vec3 operator + (const Vec3& rhs) const;
	Vec3 operator - (const Vec3& rhs) const;
	Vec3 operator / (const float val) const;
	Vec3 operator * (const float val) const;
	Vec3 operator * (const Vec3& val) const;

	void operator += (const Vec3& rhs);
	void operator -= (const Vec3& rhs);
	void operator *= (const float val);
	void operator /= (const float val);

	float getLength() const;
	Vec3 normalize() const;

	float dot_product(const Vec3& vec) const;
	Vec3 cross_product(const Vec3& vec) const;
};