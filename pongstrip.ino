#include <PongStripLib.h>


/*
 * AUTHORS: shguro, danb
 */

#include <Adafruit_NeoPixel.h>
//#include <TimerOne.h>
#include <SPI.h>

#define STRIP_PIN 6

#define POTI_ONE_PIN A0
#define POTI_TWO_PIN A5

/* helper classes */

/* colors */
Color black(0, 0, 0);

Color lifeColor(128, 0, 0);

Color topColor(0, 0, 255);
Color bottomColor(0, 255, 0);

bool hsbMode = false;

/* game */
// time per frame
uint64_t lastTime = 0;

// scale factor between virtuaintl playfield and the display
const int scaleFactor = 40;

// size of the display
const int displaySize = 60;

// paddle size of the player, actually th color tolerance
int playerSize = 40 * scaleFactor;

// amount of lifes of each player
int playerLifes = 3;

// virtual playfield dimensions
int playfieldWidth = (displaySize - 2 * playerLifes) * scaleFactor;
int playfieldHeight = 255 * scaleFactor;

Vector playfieldSize(playfieldWidth, playfieldHeight);

// frame buffer
Color display[displaySize];

// ball position and speed in the virtual playfield to be reset to
Vector ballPositionResetValue(playfieldWidth / 2, playfieldHeight / 2);
Vector ballSpeedResetValue(-2, -6);

// ball position and speed in the virtual playfield
Vector ballPosition = ballPositionResetValue;
Vector ballSpeed = ballSpeedResetValue;

// player one position and speeed in the virtual playfield
Vector playerOnePosition(0, playfieldHeight / 2);

// player one current size and lifes
int playerOneSize = playerSize;
int playerOneLifes = playerLifes;

// player two position and speeed in the virtual playfield
Vector playerTwoPosition(playfieldWidth - 1, playfieldHeight / 2);

// player two current size and lifes
int playerTwoSize = playerSize;
int playerTwoLifes = playerLifes;

// game status and winner
bool gameOver = true;
int winner = 0;

// display
Adafruit_NeoPixel strip = Adafruit_NeoPixel(displaySize, STRIP_PIN, NEO_GRB + NEO_KHZ800);

//Coin acceptor
bool newCoin = false;
byte roundCoin = 0;
byte coinPulse = 0;
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
	if (ballPosition.x + ballSpeed.x <= playerOnePosition.x + scaleFactor - 1)
	{
		// will the ball hit the paddle or will it get behind the it
		if (ballPosition.y + ballSpeed.y >= playerOnePosition.y - playerOneSize &&
		    ballPosition.y + ballSpeed.y <= playerOnePosition.y + playerOneSize)
		{
			// ball will hit the paddle, so swap the x direction and make it faster
			ballSpeed.x *= -1;
			ballSpeed.x += 1;
		}
		else
		{
			// ball will not hit the paddle, so decrement player one's lifes
			playerOneLifes--;
			
			// reset the balls position and speed and switch it's speed direction for the next round
			ballPosition = Vector(playfieldWidth / 2, playfieldHeight / 2);
			ballSpeed = ballSpeedResetValue;
			ballSpeed *= -1;
			
			// has player one any lifes left?
			if (playerOneLifes == 0)
			{
				// player one has no more lifes, player two wins, the game is over
				gameOver = true;
				winner = 2;
				
				playerOneLifes = playerLifes;
				playerTwoLifes = playerLifes;
			}
		}
	}
	
	// check player two collisions
	// will the ball be behind player two after the next iteration?
	if (ballPosition.x + ballSpeed.x >= playerTwoPosition.x - scaleFactor)
	{
		// will the ball hit the paddle or will it get behind the it
		if (ballPosition.y + ballSpeed.y >= playerTwoPosition.y - playerTwoSize &&
		    ballPosition.y + ballSpeed.y <= playerTwoPosition.y + playerTwoSize)
		{
			// ball will hit the paddle, so swap the x direction and make it faster
			ballSpeed.x *= -1;
			ballSpeed.x += -1;
		}
		else
		{
			// ball will not hit the paddle, so decrement player two's lifes
			playerTwoLifes--;
			
			// reset the balls position and speed and switch it's speed direction for the next round
			ballPosition = Vector(playfieldWidth / 2, playfieldHeight / 2);
			ballSpeed = ballSpeedResetValue;
			ballSpeed *= 1;
			
			// has player two any lifes left?
			if (playerTwoLifes == 0)
			{
				// player two has no more lifes, player one wins, the game is over
				gameOver = true;
				winner = 1;
				
				playerOneLifes = playerLifes;
				playerTwoLifes = playerLifes;
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
}

/*
 * update the user inputs
 */
void updateInputs()
{
	// read player one input
	playerOnePosition.y = (analogRead(POTI_ONE_PIN) * scaleFactor) / 4;
	
	// read player two input
	playerTwoPosition.y = (analogRead(POTI_TWO_PIN) * scaleFactor) / 4;
	
	// will player one be over the top or below the bottom after the next iteration?
	if (playerOnePosition.y - playerOneSize < 0)
	{
		// player one will be over the top, so set it to the lowest possible value
		playerOnePosition.y = playerOneSize;
	}
	else if (playerOnePosition.y + playerOneSize >= playfieldHeight)
	{
		// player one will be below the bottom, so set it to the biggest possible value
		playerOnePosition.y = playfieldHeight - playerOneSize - 1;
	}
	
	// will player two be over the top or below the bottom after the next iteration?
	if (playerTwoPosition.y - playerTwoSize < 0)
	{
		// player two will be over the top, so set it to the lowest possible value
		playerTwoPosition.y = playerTwoSize;
	}
	else if (playerTwoPosition.y + playerTwoSize >= playfieldHeight)
	{
		// player two will be below the bottom, so set it to the biggest possible value
		playerTwoPosition.y = playfieldHeight - playerTwoSize - 1;
	}
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
 * weird implementation, but somehow does, what we want to do
 */
Color hsbToRGB(int h, int s, int b) 
{ 
	int hStep = 51;
	int base;
	
	Color result;

  	if (s == 0)
  	{ 
    	result.r = b;
	    result.g = b;
	    result.b = b; 
	} 
	else
	{ 
		base = ((255 - s) * b) >> 8;

	    switch (h / hStep) 
	    { 
		    case 0: //red
		    {
		        result.r = b;		
		        result.g = (((b - base) * h) / hStep) + base;		
		        result.b = base;		
		    	break;
			}

		    case 1: //yellow
		    {
		        result.r = (((b - base) * (hStep - (h % hStep))) / hStep) + base;		
		        result.g = b;		
		        result.b = base;		
		    	break;
		    }

		    case 2: //green
		    {
		        result.r = base;		
		        result.g = b;		
		        result.b = (((b - base) * (h % hStep)) / hStep) + base;		
		    	break;
			}

			case 3: //cyan
		    {
		        result.r = base;		
		        result.g = ((b * (hStep - (h % hStep))) / hStep) + base;		
		        result.b = b;		
		    	break;
		    }

			case 4: //blue
		    {
		        result.r = (((b - base) * (h % hStep)) / hStep) + base;		
		        result.g = base;		
		        result.b = b;		
		    	break;
		    }

			case 5: //magenta
		    {
		        result.r = b;		
		        result.g = base;		
		        result.b = (((b - base) * (hStep - (h % hStep))) / hStep) + base;		
		    	break;
		    }		
		}
	}
	
	return result;
}

/*
 * generate a color acoording to the vertical position on the virtual playfield based on hsb colors
 */
Color generateHSBColorByPosition(Vector& position)
{
	Vector positionOnDisplay = position / scaleFactor;
	uint16_t yPosition = (uint16_t) positionOnDisplay.y;
	
	uint16_t yPositionHue = (yPosition * (51 * 6)) / 256;
	
	return hsbToRGB(yPositionHue, 255, 255);
}

/*
 * generate a color acoording to the vertical position on the virtual playfield based on two colors
 */
Color generateColorByPosition(Vector& position, Color& colorOne, Color& colorTwo)
{
	// return the color based on the vertical position on the virtual playfield
	// and normalized with the virtual playfields height
	//return colorOne + ((colorTwo - colorOne) * position.y) / playfieldHeight;
	
	Vector positionOnDisplay = position / scaleFactor;
	
	uint16_t rOne = (uint16_t) colorOne.r;
	uint16_t gOne = (uint16_t) colorOne.g;
	uint16_t bOne = (uint16_t) colorOne.b;
	
	uint16_t rTwo = (uint16_t) colorTwo.r;
	uint16_t gTwo = (uint16_t) colorTwo.g;
	uint16_t bTwo = (uint16_t) colorTwo.b;
	
	uint16_t yPosition = (uint16_t) positionOnDisplay.y;
	uint16_t yNorm = (uint16_t) 256;
	
	uint16_t newR = (rOne * (yNorm - yPosition) + rTwo * yPosition) / yNorm;
	uint16_t newG = (gOne * (yNorm - yPosition) + gTwo * yPosition) / yNorm;
	uint16_t newB = (bOne * (yNorm - yPosition) + bTwo * yPosition) / yNorm;
	
	Color newColor(newR, newG, newB);
	
	return newColor;
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
	
	Color ballColor;
	
	if (!hsbMode)
	{
		ballColor = generateColorByPosition(ballPosition, topColor, bottomColor);
	}
	else
	{
		ballColor = generateHSBColorByPosition(ballPosition);
	}
	
	// set color on the display
	display[playerLifes + ballDisplayPosition.x] = ballColor;
}

/*
 * render players
 */
void renderPlayers()
{
	// determine the position of player one on the display
	Vector playerOneDisplayPosition = playerOnePosition / scaleFactor;
	
	Color playerOneColor;
	
	if (!hsbMode)
	{
		playerOneColor = generateColorByPosition(playerOnePosition, topColor, bottomColor);
	}
	else
	{
		playerOneColor = generateHSBColorByPosition(playerOnePosition);
	}
	
	// set color on the display
	display[playerLifes + playerOneDisplayPosition.x] = playerOneColor;
	
	// determine the position of player two on the display
	Vector playerTwoDisplayPosition = playerTwoPosition / scaleFactor;
	
	Color playerTwoColor;
	
	if (!hsbMode)
	{
		playerTwoColor = generateColorByPosition(playerTwoPosition, topColor, bottomColor);
	}
	else
	{
		playerTwoColor = generateHSBColorByPosition(playerTwoPosition);
	}
	
	// set color on the display
	display[playerLifes + playerTwoDisplayPosition.x] = playerTwoColor;
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
	int colorDevider = 4;
	
	// loop through all pixels
	for (int i = 0; i < displaySize; i++)
	{
		// set the pixel color
		strip.setPixelColor(i, strip.Color(display[i].r / colorDevider, display[i].g / colorDevider, display[i].b / colorDevider));
	}
	
	// tell the strip to show everything
	strip.show();
}

void pause()
{
	static unsigned int counter = 0;
	
	if (counter % 16 == 0)
	{
		static int one = 0, two = 59;
		static int oneDirection = 1, twoDirection = -1;
		static unsigned int oneColorIndex = 0, twoColorIndex = 3;
		
		static const Color colors[] = { Color(255, 0, 0), Color(255, 255, 0),
		                                Color(0, 255, 0), Color(0, 255, 255),
		                                Color(0, 0, 255), Color(255, 0, 255) };
		
		Color oneColor(colors[oneColorIndex]), twoColor(colors[twoColorIndex]);
		
		for (int i = 0; i < displaySize; i++)
		{
			display[i] /= 2;
		}
		
		if ((one < two && one + oneDirection > two + twoDirection) ||
		    (one > two && one + oneDirection < two + twoDirection))
		{
			oneColorIndex++;
			oneColorIndex %= 6;
			
			twoColorIndex++;
			twoColorIndex %= 6;
		}
		
		one += oneDirection;
		two += twoDirection;
		
		oneColor = colors[oneColorIndex];
		twoColor = colors[twoColorIndex];
		
		if (one <= 0 || one >= 59)
		{
			oneDirection *= -1;
		}
		
		if (two <= 0 || two >= 59)
		{
			twoDirection *= -1;
		}
		
		if (one != two)
		{
			display[one] = oneColor;
			display[two] = twoColor;
		}
		else
		{
			display[one] = (oneColor + twoColor) / 2;
		}
	}
	
	static int lightningPart = 0;
	static const Color lightning(200, 200, 200);
	
	if (counter % 0xFFF == 0)
	{
		lightningPart = 100;
	}
	
	lightningPart--;
	
	if (lightningPart < 0)
	{
		lightningPart = 0;
	}
	
	for (int i = 0; i < displaySize; i++)
	{
		display[i] = ((100 - lightningPart) * display[i] + lightningPart * lightning) / 100;
	}
	
	counter++;
}

/*
 * reset the game to start a new one
 */
void resetGame()
{
	ballPosition = Vector(playfieldWidth / 2, playfieldHeight / 2);
	ballSpeed = ballSpeedResetValue;
	
	gameOver = false;
	winner = 0;
	
	playerOneLifes = playerLifes;
	playerTwoLifes = playerLifes;
	
	playfieldWidth = (displaySize - 2 * playerLifes) * scaleFactor;
	playfieldHeight = 255 * scaleFactor;
	
	playfieldSize = Vector(playfieldWidth, playfieldHeight);
	
	playerOnePosition = Vector(0, playfieldHeight / 2);
	playerTwoPosition = Vector(playfieldWidth - 1, playfieldHeight / 2);
}

/*
 * reset the game to start a new one and new maximum player lifes
 */
void resetGame(int _playerLifes)
{
	playerLifes = _playerLifes;
	
	resetGame();
}

/*
 * check if coins have been accepted
 */
void coinRoutine()
{
	if (newCoin)
	{
		if (roundCoin > 100)
		{
			// set lifes and start game
			if(coinPulse >= 2)
			{
				resetGame(3);
			}
			else if (coinPulse == 1)
			{
				resetGame(1);
			}
			
			// reset coin status
			coinPulse = 0;
			newCoin = false;
			roundCoin = 0;
		}
		else
		{
			roundCoin++;
		}
	}
}

/*
 * handle interrupts by the coin acceptor
 */
void coinISR()
{
	coinPulse++;
	
	newCoin = true;
}

/*
 * go through one frame iteration
 */
void gameLoop()
{
	lastTime = micros();
	
	if (!gameOver)
	{
		update();
		render();
	}
	else
	{
		pause();
	}
	
	draw();
	
	coinRoutine();
	
	while (micros() - lastTime <= 20000);
	
	Serial.println(String(int(micros() - lastTime)));
}

void setup()
{
	// set up potentiometers to work as inputs
	//pinMode(POTI_ONE_PIN, INPUT);
	//pinMode(POTI_TWO_PIN, INPUT);
	
	// set up the strip to work as output
	pinMode(STRIP_PIN, OUTPUT);
	
	// start the serial communication at 9600 baud
	Serial.begin(9600);
	
	// start the strip to actually display colors
	strip.begin();
	strip.show();
	
	//Timer1.initialize(10000);
	//Timer1.attachInterrupt(gameLoop);
	
	// sdd interrupt for coin acceptor
	attachInterrupt(0, coinISR, RISING);
}
 
void loop()
{
	// call the game iteration method in loop
	gameLoop();
}
