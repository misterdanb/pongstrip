#include <Adafruit_NeoPixel.h>
#include <TimerOne.h>
#include <SPI.h>

#define STRIP_PIN 6

#define POTI_ONE_PIN A0
#define POTI_TWO_PIN A1

/* helper classes */

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

/* colors */
Color black(0, 0, 0);

Color lifeColor(128, 0, 0);

Color topColor(0, 0, 255);
Color bottomColor(0, 255, 0);

/* game */
// scale factor between virtual playfield and the display
const int scaleFactor = 40;

// size of the display
const int displaySize = 60;

// paddle size of the player, actually th color tolerance
const int playerSize = 10000;

// amount of lifes of each player
const int playerLifes = 3;

// virtual playfield dimensions
const int playfieldWidth = (displaySize - 2 * playerLifes) * scaleFactor;
const int playfieldHeight = 255 * scaleFactor;

const Vector playfieldSize(playfieldWidth, playfieldHeight);

// frame buffer
Color display[displaySize];

// ball position and speed in the virtual playfield
Vector ballPosition(playfieldWidth / 2, playfieldHeight / 2);
Vector ballSpeed(-8, -4);

// player one position and speeed in the virtual playfield
Vector playerOnePosition(0, playfieldHeight / 2);
Vector playerOneSpeed(0, 0);

// player one current size and lifes
int playerOneSize = playerSize;
int playerOneLifes = playerLifes;

// player two position and speeed in the virtual playfield
Vector playerTwoPosition(playfieldWidth - 1, playfieldHeight / 2);
Vector playerTwoSpeed(0, 0);

// player two current size and lifes
int playerTwoSize = playerSize;
int playerTwoLifes = playerLifes;

// game status and winner
bool gameOver = false;
int winner = 0;

// display
Adafruit_NeoPixel strip = Adafruit_NeoPixel(displaySize, STRIP_PIN, NEO_GRB + NEO_KHZ800);

/*
 * checks collisions between ball and walls and ball and players
 */
void updateCollisions()
{
	// check ball collisions
	if (ballPosition.y + ballSpeed.y < 0 || ballPosition.y + ballSpeed.y >= playfieldHeight)
	{
		ballSpeed.y *= -1;
	}
	
	// check player one collisions
	// will the ball be behind player one after the next iteration?
	if (ballPosition.x + ballSpeed.x <= playerOnePosition.x)
	{
		// will the ball hit the paddle or will it get behind the it
		if (ballPosition.y + ballSpeed.y >= playerOnePosition.y - playerOneSize &&
		    ballPosition.y + ballSpeed.y <= playerOnePosition.y + playerOneSize)
		{
			// ball will hit the paddle, so swap the x direction
			ballSpeed.x *= -1;
		}
		else
		{
			// ball will not hit the paddle, so decrement player one's lifes
			playerOneLifes--;
			
			ballPosition = Vector(playfieldWidth / 2, playfieldHeight / 2);
			ballSpeed *= -1;
			
			// has player one any lifes left?
			if (playerOneLifes == 0)
			{
				// player one has no more lifes, player two wins, the game is over
				gameOver = true;
				winner = 2;
			}
		}
	}
	
	// check player two collisions
	// will the ball be behind player two after the next iteration?
	if (ballPosition.x + ballSpeed.x >= playerTwoPosition.x)
	{
		// will the ball hit the paddle or will it get behind the it
		if (ballPosition.y + ballSpeed.y >= playerTwoPosition.y - playerTwoSize &&
		    ballPosition.y + ballSpeed.y <= playerTwoPosition.y + playerTwoSize)
		{
			// ball will hit the paddle, so swap the x direction
			ballSpeed.x *= -1;
		}
		else
		{
			// ball will not hit the paddle, so decrement player two's lifes
			playerTwoLifes--;
			
			ballPosition = Vector(playfieldWidth / 2, playfieldHeight / 2);
			ballSpeed *= -1;
			
			// has player two any lifes left?
			if (playerTwoLifes == 0)
			{
				// player two has no more lifes, player one wins, the game is over
				gameOver = true;
				winner = 1;
			}
		}
	}
}

/*
 * updates the positions of ball and players
 */
void updatePositions()
{
	// update ball position
	ballPosition += ballSpeed;
	
	// update player one position
	// will player one be over the top or below the bottom after the next iteration?
	if (playerOnePosition.y - playerOneSize + playerOneSpeed.y < 0)
	{
		// player one will be over the top, so set it to the lowest possible value
		playerOnePosition.y = playerOneSize;
	}
	else if (playerOnePosition.y + playerOneSize + playerOneSpeed.y >= playfieldWidth)
	{
		// player one will be below the bottom, so set it to the biggest possible value
		playerOnePosition.y = playfieldWidth - playerOneSize - 1;
	}
	else
	{
		// player one will be within the legal range, so just add the current speed
		playerOnePosition += playerOneSpeed;
	}
	
	// update player two position
	// will player two be over the top or below the bottom after the next iteration?
	if (playerTwoPosition.y - playerTwoSize + playerTwoSpeed.y < 0)
	{
		// player two will be over the top, so set it to the lowest possible value
		playerTwoPosition.y = playerTwoSize;
	}
	else if (playerTwoPosition.y + playerTwoSize + playerTwoSpeed.y >= playfieldWidth)
	{
		// player two will be below the bottom, so set it to the biggest possible value
		playerTwoPosition.y = playfieldWidth - playerTwoSize - 1;
	}
	else
	{
		// player two will be within the legal range, so just add the current speed
		playerTwoPosition += playerTwoSpeed;
	}
}

/*
 * update the user inputs
 */
void updateInputs()
{
	// read player one input
	playerOneSpeed.y = analogRead(POTI_ONE_PIN) - 512;
	
	// read player two input
	playerTwoSpeed.y = analogRead(POTI_TWO_PIN) - 512;
}

/*
 * update everything that needs to be updated
 */
void update()
{
	updateCollisions();
	updatePositions();
	updateInputs();
}

/*
 * generate a color acoording to the vertical position on the virtual playfield
 */
Color generateColorByPosition(Vector& position, Color& colorOne, Color& colorTwo)
{
	// return the color based on the vertical position on the virtual playfield
	// and normalized with the virtual playfields height
	return colorOne + ((colorTwo - colorOne) * position.y) / playfieldHeight;
}

/*
 * render effects in the pre-object-rendering-phase
 */
void renderPreEffects()
{
	// some slow outfading of the colors on the display
	for (int i = 0; i < displaySize; i++)
	{
		display[i] /= 2;
	}
}

/*
 * render the lifes of the players
 */
void renderLifesOfThePlayers()
{
	// loop through all the possible life pixels
	for (int i = 0; i < playerLifes; i++) 
	{
		// is i in range of lifes left for player one?
		if (i < playerOneLifes)
		{
			// display the life pixel
			display[i] = lifeColor;
		}
		else
		{
			// no more life, set the pixel to black
			display[i] = black;
		}
		
		// is i in range of lifes left for player two?
		if (i < playerTwoLifes)
		{
			// display the life pixel
			display[displaySize - i - 1] = lifeColor;
		}
		else
		{
			// no more life, set the pixel to black
			display[displaySize - i - 1] = black;
		}
	}
}

/*
 * render the ball
 */
void renderBall()
{
	// determine the position of the ball on the display
	Vector ballDisplayPosition = ballPosition / scaleFactor;
	
	// set color on the display
	display[playerLifes + ballDisplayPosition.x] = generateColorByPosition(ballPosition, topColor, bottomColor);
}

/*
 * render players
 */
void renderPlayers()
{
	// determine the position of player one on the display
	Vector playerOneDisplayPosition = playerOnePosition / scaleFactor;
	
	// set color on the display
	display[playerLifes + playerOneDisplayPosition.x] = generateColorByPosition(playerOnePosition, topColor, bottomColor);
	
	// determine the position of player two on the display
	Vector playerTwoDisplayPosition = playerTwoPosition / scaleFactor;
	
	// set color on the display
	display[playerLifes + playerTwoDisplayPosition.x] = generateColorByPosition(playerTwoPosition, topColor, bottomColor);
}

/*
 * render effects in the post-object-rendering-phase
 */
void renderPostEffects()
{
}

/*
 * render everything that needs to be rendered
 */
void render()
{
	renderPreEffects();
	
	renderLifesOfThePlayers();
	
	renderBall();
	renderPlayers();
	
	renderPostEffects();
}

/*
 * draw the frame buffer on the strip
 */
void draw()
{
	// loop through all pixels
	for (int i = 0; i < displaySize; i++)
	{
		// set the pixel color
		strip.setPixelColor(i, strip.Color(display[i].r, display[i].g, display[i].b));
	}
	
	// tell the strip to show everything
	strip.show();
}

/*
 * go through one frame iteration
 */
void gameLoop()
{
	update();
	render();
	draw();
}

void setup()
{
	// set up potentiometers to work as inputs
	pinMode(POTI_ONE_PIN, INPUT);
	pinMode(POTI_TWO_PIN, INPUT);
	
	// set up the strip to work as output
	pinMode(STRIP_PIN, OUTPUT);
	
	// start the serial communication at 9600 baud
	Serial.begin(9600);
	
	// start the strip to actually display colors
	strip.begin();
	strip.show();
	
	//Timer1.initialize(10000);
	//Timer1.attachInterrupt(gameLoop);
}

void loop()
{
	// call the game iteration method in loop
	gameLoop();
	
	// delay for some time
	delay(20);
}
