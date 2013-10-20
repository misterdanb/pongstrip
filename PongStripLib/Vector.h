class Vector
{
public:
	int x, y;

	Vector();
	
	Vector(int newX, int newY);
	
	Vector operator+(const Vector& right);
	
	Vector& operator+=(const Vector& right);
	
	Vector operator-(const Vector& right);
	
	Vector& operator-=(const Vector& right);
	
	friend Vector operator*(const Vector& left, int right)
	{
		return Vector(left.x * right, left.y * right);
	}

	friend Vector operator*(int left, const Vector& right)
	{
		return Vector(left * right.x, left * right.y);
	}
	
	Vector& operator*=(int right);
	
	friend Vector operator/(const Vector& left, int right)
	{
		return Vector(left.x / right, left.y / right);
	}

	
	Vector& operator/=(int right);
	

};
