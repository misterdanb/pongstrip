#include <Adafruit_NeoPixel.h>
#include <TimerOne.h>
#include <SPI.h>

#define STRIP_PIN 6

#define POTI_ONE_PIN A0
#define POTI_TWO_PIN A1

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

// colors
Color black(0, 0, 0);

Color lifeColor(128, 0, 0);

Color topColor(0, 0, 255);
Color bottomColor(0, 255, 0);

// game
const int scaleFactor = 128;

const int displaySize = 60;

const int playerSize = 10;
const int playerLifes = 3;

const int playfieldWidth = (displaySize - 2 * playerLifes) * scaleFactor;
const int playfieldHeight = 255 * scaleFactor;

const Vector playfieldSize(playfieldWidth, playfieldHeight);

Color display[displaySize];

Vector ballPosition(playfieldWidth / 2, playfieldHeight / 2);
Vector ballSpeed(-100, -800);

Vector playerOnePosition(0, playfieldHeight / 2);
Vector playerOneSpeed(0, 0);
int playerOneSize = playerSize;
int playerOneLifes = playerLifes;

Vector playerTwoPosition(playfieldWidth - 1, playfieldHeight / 2);
Vector playerTwoSpeed(0, 0);
int playerTwoSize = playerSize;
int playerTwoLifes = playerLifes;

bool gameOver = false;
int winner = 0;

// display
Adafruit_NeoPixel strip = Adafruit_NeoPixel(displaySize, STRIP_PIN, NEO_RGB + NEO_KHZ800);

void updateCollisions()
{
	// check ball collisions
	if (ballPosition.y + ballSpeed.y < 0 || ballPosition.y + ballSpeed.y >= playfieldHeight)
	{
		ballSpeed.y *= -1;
	}
	
	// check player one collisions
	if (ballPosition.x + ballSpeed.x <= playerOnePosition.x)
	{
		if (ballPosition.y + ballSpeed.y >= playerOnePosition.y - playerOneSize &&
		    ballPosition.y + ballSpeed.y <= playerOnePosition.y + playerOneSize)
		{
			ballSpeed.x *= -1;
		}
		else
		{
			playerOneLifes--;
			
			if (playerOneLifes == 0)
			{
				gameOver = true;
				winner = 2;
			}
		}
	}
	
	// check player two collisions
	if (ballPosition.x + ballSpeed.x >= playerTwoPosition.y)
	{
		if (ballPosition.y + ballSpeed.y >= playerTwoPosition.y - playerTwoSize &&
		    ballPosition.y + ballSpeed.y <= playerTwoPosition.y + playerTwoSize)
		{
			ballSpeed.x *= -1;
		}
		else
		{
			playerTwoLifes--;
			
			if (playerTwoLifes == 0)
			{
				gameOver = true;
				winner = 1;
			}
		}
	}
}

void updatePositions()
{
	// update ball position
	ballPosition += ballSpeed;
	
	// update player one position
	if (playerOnePosition.y + playerOneSpeed.y < 0)
	{
		playerOnePosition.y = 0;
	}
	else if (playerOnePosition.y + playerOneSpeed.y >= playfieldWidth)
	{
		playerOnePosition.y = playfieldWidth - 1;
	}
	else
	{
		playerOnePosition += playerOneSpeed;
	}
	
	// update player two position
	if (playerTwoPosition.y + playerTwoSpeed.y < 0)
	{
		playerTwoPosition.y = 0;
	}
	else if (playerTwoPosition.y + playerTwoSpeed.y >= playfieldWidth)
	{
		playerTwoPosition.y = playfieldWidth - 1;
	}
	else
	{
		playerTwoPosition += playerTwoSpeed;
	}
}

void updateInputs()
{
	// read player one input
	playerOneSpeed.y = analogRead(POTI_ONE_PIN) - 512;
	
	// read player two input
	playerTwoSpeed.y = analogRead(POTI_TWO_PIN) - 512;
}

void update()
{
	updateCollisions();
	updatePositions();
	updateInputs();
}

Color generateColorByPosition(Vector& position, Color& colorOne, Color& colorTwo)
{
	return colorOne + (position.y * (colorTwo - colorOne)) / playfieldHeight;
}

void renderPreEffects()
{
	for (int i = 0; i < displaySize; i++)
	{
		display[i] /= 2;
	}
}

void renderPlayerOneLifes()
{
	switch (playerOneLifes)
	{
	case 0:
		display[0] = black;
		display[1] = black;
		display[2] = black;
		break;
		
	case 1:
		display[0] = lifeColor;
		display[1] = black;
		display[2] = black;
		break;
		
	case 2:
		display[0] = lifeColor;
		display[1] = lifeColor;
		display[2] = black;
		break;
		
	case 3:
		display[0] = lifeColor;
		display[1] = lifeColor;
		display[2] = lifeColor;
		break;
		
	default:
		display[displaySize - 1] = black;
		display[displaySize - 2] = black;
		display[displaySize - 3] = black;
		break;
	}
}

void renderPlayerTwoLifes()
{
	switch (playerTwoLifes)
	{
	case 0:
		display[displaySize - 1] = black;
		display[displaySize - 2] = black;
		display[displaySize - 3] = black;
		break;
		
	case 1:
		display[displaySize - 1] = lifeColor;
		display[displaySize - 2] = black;
		display[displaySize - 3] = black;
		break;
		
	case 2:
		display[displaySize - 1] = lifeColor;
		display[displaySize - 2] = lifeColor;
		display[displaySize - 3] = black;
		break;
		
	case 3:
		display[displaySize - 1] = lifeColor;
		display[displaySize - 2] = lifeColor;
		display[displaySize - 3] = lifeColor;
		break;
		
	default:
		display[displaySize - 1] = black;
		display[displaySize - 2] = black;
		display[displaySize - 3] = black;
		break;
	}
}

void renderBall()
{
	Vector ballDisplayPosition = ballPosition / scaleFactor;
	
	// render ball
	display[playerLifes + ballDisplayPosition.x] = generateColorByPosition(ballPosition, topColor, bottomColor);
}

void renderPlayerOne()
{
	Vector playerOneDisplayPosition = playerOnePosition / scaleFactor;
	
	// render player one
	display[playerLifes + playerOneDisplayPosition.x] = generateColorByPosition(playerOnePosition, topColor, bottomColor);
}

void renderPlayerTwo()
{
	Vector playerTwoDisplayPosition = playerTwoPosition / scaleFactor;
	
	// render player two
	display[playerLifes + playerTwoDisplayPosition.x] = generateColorByPosition(playerTwoPosition, topColor, bottomColor);
}

void renderPostEffects()
{
}

void render()
{
	renderPreEffects();
	
	renderPlayerOneLifes();
	renderPlayerTwoLifes();
	
	renderBall();
	renderPlayerOne();
	renderPlayerTwo();
	
	renderPostEffects();
}

void draw()
{
	for (int i = 0; i < displaySize; i++)
	{
		strip.setPixelColor(i, strip.Color(display[i].r, display[i].g, display[i].b));
	}
	
	strip.show();
}

void gameLoop()
{
	update();
	render();
	draw();
}

void setup()
{
	pinMode(POTI_ONE_PIN, INPUT);
	pinMode(POTI_TWO_PIN, INPUT);
	
	pinMode(STRIP_PIN, OUTPUT);
	
	Serial.begin(9600);
	
	strip.begin();
	strip.show();
	
	//Timer1.initialize(10000);
	//Timer1.attachInterrupt(gameLoop);
}

void loop()
{
	gameLoop();
	delay(1000);
}
