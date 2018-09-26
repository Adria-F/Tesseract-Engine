#pragma once
#include "Color.h"
#include "MathGeoLib/MathGeoLib.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	float4x4 transform;
	bool axis,wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class Cube : public Primitive
{
public :
	Cube();
	Cube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec size;
};

// ============================================
class MSphere : public Primitive
{
public:
	MSphere();
	MSphere(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class MCylinder : public Primitive
{
public:
	MCylinder();
	MCylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class MLine : public Primitive
{
public:
	MLine();
	MLine(float x, float y, float z);
	void InnerRender() const;
public:
	vec origin;
	vec destination;
};

// ============================================
class MPlane : public Primitive
{
public:
	MPlane();
	MPlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec normal;
	float constant;
};