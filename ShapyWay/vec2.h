#pragma once
#include <cmath>

//A quick 2D vector class
struct vec2
{
	double x;
	double y;
	
	vec2() : x(0), y(0) {}
	vec2(double x, double y) : x(x), y(y) {}

	vec2 operator+(const vec2& v) const { return vec2(x+v.x, y+v.y); }
	void operator+=(const vec2& v) { x+=v.x; y+=v.y; }
	vec2 operator-(const vec2& v) const { return vec2(x-v.x, y-v.y); }
	void operator-=(const vec2& v) { x-=v.x; y-=v.y; }
	vec2 operator*(double v) const { return vec2(x*v, y*v); } //Scalar product
	void operator*=(double v) { x*=v; y*=v; }
	vec2 operator/(double v) const { return vec2(x/v, y/v); }
	void operator/=(double v) { x /= v; y /= v; }
	double operator*(const vec2& v) const { return x*v.x + y*v.y; } //Dot product
	double operator%(const vec2& v) const { return x*v.y - y*v.x; } //Cross product (determinant)

	double mag() const { return sqrt(x*x+y*y); } //Magnitude
	double magsqr() const { return x*x+y*y; } //Magnitude squared
	vec2 normalized() const { return vec2(x/mag(), y/mag()); }
};