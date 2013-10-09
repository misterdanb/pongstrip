#ifndef __VECTOR_H__
#define __VECTOR_H__

class Vector
{
public:
	Vector()
		: x(0), y(0)
	{
	}
	
	Vector(int newX, int newY)
		: x(newX), y(newY)
	{
	}
	
	Vector operator+(const Vector& right)
	{
		return Vector(x + right.x, y + right.y);
	}
	
	Vector& operator+=(const Vector& right)
	{
		x += right.x;
		y += right.y;
		
		return (*this);
	}
	
	Vector operator-(const Vector& right)
	{
		return Vector(x - right.x, y - right.y);
	}
	
	Vector& operator-=(const Vector& right)
	{
		x -= right.x;
		y -= right.y;
		
		return (*this);
	}
	
	friend Vector operator*(const Vector& left, int right)
	{
		return Vector(left.x * right, left.y * right);
	}
	
	friend Vector operator*(int left, const Vector& right)
	{
		return Vector(left * right.x, left * right.y);
	}
	
	Vector& operator*=(int right)
	{
		x *= right;
		y *= right;
		
		return (*this);
	}
	
	friend Vector operator/(const Vector& left, int right)
	{
		return Vector(left.x / right, left.y / right);
	}
	
	Vector& operator/=(int right)
	{
		x /= right;
		y /= right;
		
		return (*this);
	}
	
	int x, y;
};

#endif
