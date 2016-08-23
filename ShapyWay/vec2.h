#pragma once
#include <cmath>

//A quick 2D vector class
struct vec2
{
	float x;
	float y;
	
	vec2() : x(0), y(0) {}
	vec2(float x, float y) : x(x), y(y) {}

	vec2 operator+(const vec2& v) const { return vec2(x+v.x, y+v.y); }
	void operator+=(const vec2& v) { x+=v.x; y+=v.y; }
	vec2 operator-(const vec2& v) const { return vec2(x-v.x, y-v.y); }
	void operator-=(const vec2& v) { x-=v.x; y-=v.y; }
	vec2 operator*(float v) const { return vec2(x*v, y*v); } //Scalar product
	void operator*=(float v) { x*=v; y*=v; }
	vec2 operator/(float v) const { return vec2(x/v, y/v); }
	void operator/=(float v) { x /= v; y /= v; }
	float operator*(const vec2& v) const { return x*v.x + y*v.y; } //Dot product
	float operator%(const vec2& v) const { return x*v.y - y*v.x; } //Cross product (determinant)

	float mag() const { return sqrt(x*x+y*y); } //Magnitude
	float magsqr() const { return x*x+y*y; } //Magnitude squared
	vec2 normalized() const { return vec2(x/mag(), y/mag()); }
};