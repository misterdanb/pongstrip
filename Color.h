#define __COLOR_H__
#define __COLOR_H__

class Color
{
public:
	Color()
		: r(0), g(0), b(0)
	{
	}
	
	Color(int newR, int newG, int newB)
		: r(newR), g(newG), b(newB)
	{
	}
	
	Color operator+(const Color& right)
	{
		return Color(r + right.r, g + right.g, b + right.b);
	}
	
	Color& operator+=(const Color& right)
	{
		r += right.r;
		g += right.g;
		b += right.b;
		
		return (*this);
	}
	
	Color operator-(const Color& right)
	{
		return Color(r - right.r, g - right.g, b - right.b);
	}
	
	Color& operator-=(const Color& right)
	{
		r -= right.r;
		g -= right.g;
		b -= right.b;
		
		return (*this);
	}
	
	friend Color operator*(const Color& left, int right)
	{
		return Color(left.r * right, left.g * right, left.b * right);
	}
	
	friend Color operator*(int left, const Color& right)
	{
		return Color(left * right.r, left * right.g, left * right.b);
	}
	
	Color& operator*=(int right)
	{
		r *= right;
		g *= right;
		b *= right;
		
		return (*this);
	}
	
	friend Color operator/(const Color& left, int right)
	{
		return Color(left.r / right, left.g / right, left.b / right);
	}
	
	Color& operator/=(int right)
	{
		r /= right;
		g /= right;
		b /= right;
		
		return (*this);
	}
	
	int r, g, b;
};

#endif
