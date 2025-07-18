/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

/****
* for rcbot:
*
* Edited : Length() Functions to use pow()
*
* Added : <= & >= Operators;
*
****/

#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

//=========================================================
// 2DVector - used for many pathfinding and many other
// operations that are treated as planar rather than 3d.
//=========================================================
class Vector2D
{
public:
	Vector2D() = default;
	Vector2D(const float X, const float Y) { x = X; y = Y; }
	Vector2D operator+(const Vector2D& v)	const { return { x + v.x, y + v.y }; }
	Vector2D operator-(const Vector2D& v)	const { return { x - v.x, y - v.y }; }
	Vector2D operator*(const float fl)				const { return { x * fl, y * fl }; }
	Vector2D operator/(const float fl)				const { return { x / fl, y / fl }; }

	float Length()						const { return std::pow(x * x + y * y, 0.5f); }//sqrt(x*x + y*y);		}

	Vector2D Normalize() const
	{
		//Vector2D vec2;

		float flLen = Length();
		if (flLen == 0.0f)
		{
			return { 0, 0 };
		}
		else
		{
			flLen = 1 / flLen;
			return { x * flLen, y * flLen };
		}
	}

	vec_t	x, y;
};

inline float DotProduct(const Vector2D& a, const Vector2D& b) { return a.x * b.x + a.y * b.y; }
inline Vector2D operator*(const float fl, const Vector2D& v) { return v * fl; }

//=========================================================
// 3D Vector
//=========================================================
class Vector						// same data-layout as engine's vec3_t,
{								//		which is a vec_t[3]
public:
	// Construction/destruction
	Vector() = default;
	Vector(const float X, const float Y, const float Z) { x = X; y = Y; z = Z; }
	//inline Vector(double X, double Y, double Z)		{ x = (float)X; y = (float)Y; z = (float)Z;	}
	//inline Vector(int X, int Y, int Z)				{ x = (float)X; y = (float)Y; z = (float)Z;	}
	Vector(const Vector& v) { x = v.x; y = v.y; z = v.z; }
	Vector(const float rgfl[3]) { x = rgfl[0]; y = rgfl[1]; z = rgfl[2]; }

	// Operators
	Vector operator-() const { return { -x, -y, -z }; }
	int operator==(const Vector& v) const { return x == v.x && y == v.y && z == v.z; }
	int operator!=(const Vector& v) const { return !(*this == v); }
	Vector operator+(const Vector& v) const { return { x + v.x, y + v.y, z + v.z }; }
	Vector operator-(const Vector& v) const { return { x - v.x, y - v.y, z - v.z }; }
	Vector operator*(const float fl) const { return { x * fl, y * fl, z * fl }; }
	Vector operator/(const float fl) const { return { x / fl, y / fl, z / fl }; }

	/////////////////////////////
	// ADDED OPERATORS => / <=
	bool operator<=(const Vector& v) const { return x <= v.x && y <= v.y && z <= v.z; }
	bool operator>=(const Vector& v) const { return x >= v.x && y >= v.y && z >= v.z; }

	// Methods
	void CopyToArray(float* rgfl) const
	{
		rgfl[0] = x;
		rgfl[1] = y;
		rgfl[2] = z;
	}

	float Length() const { return std::pow(x * x + y * y + z * z, 0.5f); }//sqrt(x*x + y*y + z*z); }
	operator float* () { return &x; } // Vectors will now automatically convert to float * when needed
	operator const float* () const { return &x; } // Vectors will now automatically convert to float * when needed
	Vector Normalize() const
	{
		float flLen = Length();
		if (flLen == 0.0f) return { 0, 0, 1 }; // ????
		flLen = 1 / flLen;
		return { x * flLen, y * flLen, z * flLen };
	}

	Vector2D Make2D() const
	{
		Vector2D	Vec2;

		Vec2.x = x;
		Vec2.y = y;

		return Vec2;
	}

	float Length2D() const { return std::sqrt(x * x + y * y); }

	// Members
	vec_t x, y, z;
};
inline Vector operator*(const float fl, const Vector& v) { return v * fl; }
inline float DotProduct(const Vector& a, const Vector& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vector CrossProduct(const Vector& a, const Vector& b) {
	return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x
	};
}
inline float VecDistance(const Vector& a, const Vector& b) { return (b - a).Length(); }

#endif