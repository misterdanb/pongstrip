
#include "Color.h"

Color::Color()
	: r(0), g(0), b(0)
{
}

Color::Color(int newR, int newG, int newB)
	: r(newR), g(newG), b(newB)
{
}

Color Color::operator+(const Color& right)
{
	return Color(r + right.r, g + right.g, b + right.b);
}

Color& Color::operator+=(const Color& right)
{
	r += right.r;
	g += right.g;
	b += right.b;
	
	return (*this);
}

Color Color::operator-(const Color& right)
{
	return Color(r - right.r, g - right.g, b - right.b);
}

Color& Color::operator-=(const Color& right)
{
	r -= right.r;
	g -= right.g;
	b -= right.b;
	
	return (*this);
}



Color& Color::operator*=(int right)
{
	r *= right;
	g *= right;
	b *= right;
	
	return (*this);
}



Color& Color::operator/=(int right)
{
	r /= right;
	g /= right;
	b /= right;
	
	return (*this);
}
	
