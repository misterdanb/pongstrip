
class Color
{
public:
	int r, g, b;
	Color();
	Color(int newR, int newG, int newB);
	
	Color operator+(const Color& right);
	
	Color& operator+=(const Color& right);
	
	Color operator-(const Color& right);
	
	Color& operator-=(const Color& right);
	
	friend Color operator*(const Color& left, int right)
	{
		return Color(left.r * right, left.g * right, left.b * right);
	}

	friend Color operator*(int left, const Color& right)
	{
		return Color(left * right.r, left * right.g, left * right.b);
	}
	
	Color& operator*=(int right);
	
	friend Color operator/(const Color& left, int right)
	{
		return Color(left.r / right, left.g / right, left.b / right);
	}
	
	Color& operator/=(int right);
	
};
