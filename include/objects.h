#ifndef OBJECTS_H
#define OBJECTS_H

class Point
{
public:
  int x, y;
  Point() {}
  Point(int x_, int y_) : x(x_), y(y_) {}
};

class Point3d
{
public:
  int x, y, z;
  Point3d() {}
  Point3d(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
};



class Vector
{
public:
	float x, y;
	Vector(float x_, float y_) : x(x_), y(y_) {}
};

class PixelNode {
  int px;
  int py;
  float value;
public:
	PixelNode()		: px(0), py(0), value(0) {}
	PixelNode(int i): px(i), py(i), value(0) {}
	PixelNode(int x, int y, float v) { 
		px = x; 
		py = y; 
		value = v; 
	}
	PixelNode(int x, int y) { 
		px = x; 
		py = y; 
		value = 0; 
	}
	PixelNode( const PixelNode& p)	: px(p.px), py(p.py), value(p.value) {}
	
	bool operator< (const PixelNode &b) const	{
	  return value < b.getValue();
	}
	bool operator> (const PixelNode &b) const	{
	  return value > b.getValue();
	}
	bool operator==( const PixelNode& pt ) const { 
		return  pt.getX() == px && pt.getY() == py; 
	}
	bool operator!=( const PixelNode& pt ) const { 
		return !(pt==*this); 
	}	
	PixelNode operator+(const PixelNode& pt ) const { 
		return PixelNode(px+pt.getX(),py+pt.getY()); 
	}
	PixelNode& operator+=( const PixelNode& pt ) { 
		px += pt.getX(); 
		py += pt.getY(); 
		return *this; 
	}	
	float getValue() const { 
		return value; 
	}
	int getX() const {
		return px;
	}
	int getY() const {
		return py;
	}
};

#endif
