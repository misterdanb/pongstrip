#include "Vector.h"
Vector::Vector()
	: x(0), y(0)
{
}
Vector::Vector(int newX, int newY)
	: x(newX), y(newY)
{
}

Vector Vector::operator+(const Vector& right)
{
	return Vector(x + right.x, y + right.y);
}

Vector& Vector::operator+=(const Vector& right)
{
	x += right.x;
	y += right.y;
	
	return (*this);
}

Vector Vector::operator-(const Vector& right)
{
	return Vector(x - right.x, y - right.y);
}

Vector& Vector::operator-=(const Vector& right)
{
	x -= right.x;
	y -= right.y;
	
	return (*this);
}



Vector& Vector::operator*=(int right)
{
	x *= right;
	y *= right;
	
	return (*this);
}


Vector& Vector::operator/=(int right)
{
	x /= right;
	y /= right;
	
	return (*this);
}

