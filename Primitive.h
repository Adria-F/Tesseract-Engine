#pragma once
#include "Color.h"
#include <vector>
#include "MathGeoLib/MathGeoLib.h"

using namespace std;

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
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

protected:
	
	void generateBuffer();

public:
	
	Color color;
	float4x4 transform;
	bool axis,wire;

	vector<float> shape;
	vector<uint> indices;

	uint my_id = 0;

protected:
	
	PrimitiveTypes type;

};

// ============================================
class MCube : public Primitive
{
public :
	MCube();
	MCube(float sizeX, float sizeY, float sizeZ, vec center);
public:
	vec size;
};

// ============================================
class MSphere : public Primitive
{
public:
	MSphere();
	MSphere(float radius, int rings, int sectors, vec pos);
public:
	float radius;
};

// ============================================
class MCylinder : public Primitive
{
public:
	MCylinder();
	MCylinder(float radius, float height,int rings, int sectors, vec pos);
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
	void Render() const;
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

	void Render() const;
public:
	vec normal;
	float constant;
};