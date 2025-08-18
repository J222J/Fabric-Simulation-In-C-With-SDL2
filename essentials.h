#pragma once

#include<cmath>

class pos2 {
public:
	float x, y;

private:
	void rotate_origin(float angle) {
		float angle_rad = -angle * M_PI / 180.0f;
	
		float new_x = x * cos(angle_rad) - y * sin(angle_rad);
		float new_y = x * sin(angle_rad) + y * cos(angle_rad);

		x = new_x;
		y = new_y;
	}

public:
	void rotate(float angle, pos2 position = pos2{0.0f, 0.0f}) {
		x -= position.x;
		y -= position.y;

		rotate_origin(angle);

		x += position.x;
		y += position.y;
	}

	pos2(float xx, float yy) {
		x = xx;
		y = yy;
	}

	pos2() {
		x = 0.0f;
		y = 0.0f;
	}

	~pos2() = default;
};

class pos3 {
public:
	float x, y, z;

private:
	void rotate_origin_x(float angle) {
		float angle_rad = -angle * M_PI / 180.0f;

		float new_z = z * cos(angle_rad) - y * sin(angle_rad);
		float new_y = z * sin(angle_rad) + y * cos(angle_rad);

		z = new_z;
		y = new_y;
	}

	void rotate_origin_y(float angle) {
		float angle_rad = -angle * M_PI / 180.0f;

		float new_x = x * cos(angle_rad) - z * sin(angle_rad);
		float new_z = x * sin(angle_rad) + z * cos(angle_rad);

		x = new_x;
		z = new_z;
	}
	
	void rotate_origin_z(float angle) {
		float angle_rad = -angle * M_PI / 180.0f;

		float new_x = x * cos(angle_rad) - y * sin(angle_rad);
		float new_y = x * sin(angle_rad) + y * cos(angle_rad);

		x = new_x;
		y = new_y;
	}

public:
	void rotate(float x_angle, float y_angle, float z_angle, pos3 position = pos3{0.0f, 0.0f, 0.0f}) {
		x -= position.x;
		y -= position.y;
		z -= position.z;

		rotate_origin_x(x_angle);
		rotate_origin_y(y_angle);
		rotate_origin_z(z_angle);

		x += position.x;
		y += position.y;
		z += position.z;
	}

	pos3(float xx, float yy, float zz) {
		x = xx;
		y = yy;
		z = zz;
	}

	pos3() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	~pos3() = default;
};

float distance(pos2 a, pos2 b) {
	float dx = b.x-a.x;
	float dy = b.y-a.y;

	return sqrt(dx*dx + dy*dy);
}

float distance(pos3 a, pos3 b) {
	float dx = b.x-a.x;
	float dy = b.y-a.y;
	float dz = b.z-a.z;

	return sqrt(dx*dx + dy*dy + dz*dz);
}

pos2 operator+(pos2 a, pos2 b) {
	return pos2{a.x+b.x, a.y+b.y};
}

pos3 operator+(pos3 a, pos3 b) {
	return pos3{a.x+b.x, a.y+b.y, a.z+b.z};
}

void operator+=(pos2 &a, pos2 b) {
	a.x += b.x;
	a.y += b.y;
}

void operator+=(pos3 &a, pos3 b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}

class vec2 {
public:
	float x, y;

	float length() {
		return sqrt(x*x + y*y);
	}

	vec2 normalized() {
		float len = length();
		if (len == 0.0f)
			return vec2{0, 0};

		return vec2{x/len, y/len};
	}

	void normalize() {
		float len = length();
		if (len == 0.0f)
			return;
		x /= len;
		y /= len;
	}

	void rotate(float angle) { // in degrees and clockwise
		float angle_rad = -angle * M_PI / 180.0f;

		float new_x = x * cos(angle_rad) - y * sin(angle_rad);
		float new_y = x * sin(angle_rad) + y * cos(angle_rad);
	
		x = new_x;
		y = new_y;
	}

	vec2(float xx, float yy) {
		x = xx;
		y = yy;
	}

	vec2(pos2 a, pos2 b) {
		x = b.x-a.x;
		y = b.y-a.y;
	}

	vec2() {
		x = 0.0f;
		y = 0.0f;
	}

	~vec2() = default;
};

class vec3 {
public:
	float x, y, z;

private:
	void rotate_x(float angle) {
		float angle_rad = -angle * M_PI / 180.0f;

		float new_z = z * cos(angle_rad) - y * sin(angle_rad);
		float new_y = z * sin(angle_rad) + y * cos(angle_rad);
	
		z = new_z;
		y = new_y;
	}

	void rotate_y(float angle) {
		float angle_rad = -angle * M_PI / 180.0f;

		float new_x = x * cos(angle_rad) - z * sin(angle_rad);
		float new_z = x * sin(angle_rad) + z * cos(angle_rad);
	
		x = new_x;
		z = new_z;
	}

	void rotate_z(float angle) {
		float angle_rad = -angle * M_PI / 180.0f;

		float new_x = x * cos(angle_rad) - y * sin(angle_rad);
		float new_y = x * sin(angle_rad) + y * cos(angle_rad);
	
		x = new_x;
		y = new_y;
	}

public:

	float length() {
		return sqrt(x*x + y*y + z*z);
	}

	vec3 normalized() {
		float len = length();
		if (len == 0.0f)
			return vec3{0.0f, 0.0f, 0.0f};

		return vec3{x/len, y/len, z/len};
	}

	void normalize() {
		float len = length();
		if (len == 0.0f)
			return;
		
		x /= len;
		y /= len;
		z /= len;
	}

	void rotate(float x_angle, float y_angle, float z_angle) { // in degrees and clockwise
		rotate_x(x_angle);
		rotate_y(y_angle);
		rotate_z(z_angle);
	}

	vec3(float xx, float yy, float zz) {
		x = xx;
		y = yy;
		z = zz;
	}

	vec3(pos3 a, pos3 b) {
		x = b.x-a.x;
		y = b.y-a.y;
		z = b.z-a.z;
	}

	vec3() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	~vec3() = default;
};

vec2 operator+(vec2 a, vec2 b) {
	return vec2{a.x+b.x, a.y+b.y};
}

vec3 operator+(vec3 a, vec3 b) {
	return vec3{a.x+b.x, a.y+b.y, a.z+b.z};
}

void operator+=(vec2 &a, vec2 b) {
	a.x += b.x;
	a.y += b.y;
}

void operator+=(vec3 &a, vec3 b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}

float operator*(vec2 a, vec2 b) {
	return (a.x*b.x + a.y*b.y);
}

float operator*(vec3 a, vec3 b) {
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}

vec2 operator*(vec2 a, int b) {
	return vec2{a.x*(float)b, a.y*(float)b};
}

vec2 operator*(vec2 a, float b) {
	return vec2{a.x*b, a.y*b};
}

vec3 operator*(vec3 a, int b) {
	return vec3{a.x*(float)b, a.y*(float)b, a.z*(float)b};
}

vec3 operator*(vec3 a, float b) {
	return vec3{a.x*b, a.y*b, a.z*b};
}

vec2 operator*(int b, vec2 a) {
	return vec2{a.x*(float)b, a.y*(float)b};
}

vec2 operator*(float b, vec2 a) {
	return vec2{a.x*b, a.y*b};
}

vec3 operator*(int b, vec3 a) {
	return vec3{a.x*(float)b, a.y*(float)b, a.z*(float)b};
}

vec3 operator*(float b, vec3 a) {
	return vec3{a.x*b, a.y*b, a.z*b};
}

void operator*=(vec2 &a, int b) {
	a.x *= (float)b;
	a.y *= (float)b;
}

void operator*=(vec2 &a, float b) {
	a.x *= b;
	a.y *= b;
}

void operator*=(vec3 &a, int b) {
	a.x *= (float)b;
	a.y *= (float)b;
	a.z *= (float)b;
}

void operator*=(vec3 &a, float b) {
	a.x *= b;
	a.y *= b;
	a.z *= b;
}

vec2 operator/(vec2 a, int b) {
	return vec2{a.x/(float)b, a.y/(float)b};
}

vec2 operator/(vec2 a, float b) {
	return vec2{a.x/b, a.y/b};
}

vec3 operator/(vec3 a, int b) {
	return vec3{a.x/(float)b, a.y/(float)b, a.z/(float)b};
}

vec3 operator/(vec3 a, float b) {
	return vec3{a.x/b, a.y/b, a.z/b};
}

void operator/=(vec2 &a, int b) {
	a.x /= (float)b;
	a.y /= (float)b;
}

void operator/=(vec2 &a, float b) {
	a.x /= b;
	a.y /= b;
}

void operator/=(vec3 &a, int b) {
	a.x /= (float)b;
	a.y /= (float)b;
	a.z /= (float)b;
}

void operator/=(vec3 &a, float b) {
	a.x /= b;
	a.y /= b;
	a.z /= b;
}

float angle(vec2 a, vec2 b) {
	float cosine = a*b / a.length() / b.length();
	float angle = acos(cosine) * 180.0f / M_PI;

	return angle;
}

float angle(vec3 a, vec3 b) {
	float cosine = a*b / a.length() / b.length();
	float angle = acos(cosine) * 180.0f / M_PI;

	return angle;
}
