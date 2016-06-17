#include "mylib.h"
#include "text.h"
#include "startscreen.h"
#include "bluebrick.h"
#include "redbrick.h"
#include "greenbrick.h"
#include "yellowbrick.h"
#include "purplebrick.h"
#include "whitebrick.h"
#include "gtech.h"
#include "win.h"
#include "lose.h"

#include <stdio.h>

#define PADDLE_Y 		140
#define PADDLE_WIDTH 	30
#define PADDLE_HEIGHT 	7
#define PADDLE_SPEED	3
#define PADDLE_COLOR	CYAN

#define BRICK_WIDTH		20
#define BRICK_HEIGHT	10

#define BALL_SIZE		6
#define BALL_SPEED		6
#define BALL_COLOR		GREEN

#define NUM_OF_BRICKS	80

typedef struct {
	int x;
	int y;
	int health; //number of hits it takes to destroy the block
	u16 color;
} BRICK_STRUCT;

typedef struct {
	int x;
} PADDLE_STRUCT;

typedef struct {
	int x;
	int y;
	int xspeed;
	int yspeed;
	int onPaddle;
} BALL_STRUCT;

static PADDLE_STRUCT paddle;
static PADDLE_STRUCT oldPaddle;
static BALL_STRUCT ball;
static BALL_STRUCT oldBall;
static BRICK_STRUCT brickArray[NUM_OF_BRICKS];

static int bricksLeft;
static char bricksString[2];
static int lives;
static char livesString[1];

void updatePositions();
void drawEverything();

void setBallOnPaddle();
void setNewBallSpeed();

void instantiateBrickArray();

void repaintColorsUnderBall();

int main(void)
{
    //video mode 3, activate background 2
    *REG_DISPCNT = MODE3 | BG2_ENABLE;

    paddle.x = 113;
    oldPaddle.x = 113;

    //ball starts on top of paddle
    setBallOnPaddle();

    lives = 3;
   	bricksLeft = NUM_OF_BRICKS;

   	sprintf(livesString, "%d", lives);  		//store lives as a string
   	sprintf(bricksString, "%d", bricksLeft);  	//store bricks left as a string

    drawBackground3(startscreen);
    drawString(10, 125, "Press any key", WHITE);
    drawString(10, 135, "to start", WHITE);

    while(!ANY_KEY_DOWN)
    {
    	//press key to advance
    }

    fillScreen(BLACK);

    waitForVblank();
    drawRect(GAME_WIDTH, 0, SCREEN_WIDTH - GAME_WIDTH, SCREEN_HEIGHT, RGB(6, 0, 6));

    instantiateBrickArray();

    drawString(205, 20, "Lives", WHITE);
    drawString(215, 35, livesString, WHITE);

    drawString(202, 60, "Bricks", WHITE);
    drawString(208, 70, "left", WHITE);
    drawString(215, 85, bricksString, WHITE);



    while(lives > 0 && bricksLeft > 0)
    {
    	updatePositions();
    	drawEverything();
    }

    if (bricksLeft <= 0)
    {
    	drawBackground3(win);
    }
    else
    {
    	drawBackground3(lose);
    }

    while(TRUE)
    {
    	//do nothing
    }

}

void updatePositions()
{
	oldPaddle = paddle;
	oldBall = ball;

	if (KEY_DOWN_NOW(BUTTON_RIGHT))
	{
		paddle.x = min(paddle.x + PADDLE_SPEED, GAME_WIDTH - PADDLE_WIDTH);

		if (ball.onPaddle)
			ball.x = paddle.x + PADDLE_WIDTH / 2;
	}

	if (KEY_DOWN_NOW(BUTTON_LEFT))
	{
		paddle.x = max(paddle.x - PADDLE_SPEED, 0);

		if (ball.onPaddle)
			ball.x = paddle.x + PADDLE_WIDTH / 2;
	}

	//release ball from paddle
	if (ball.onPaddle && KEY_DOWN_NOW(BUTTON_A))
	{
		ball.onPaddle = FALSE;
		ball.xspeed = 1;
		ball.yspeed = -3;

		ball.y -= 1; //give it one pixel of space so it doesn't "collide" with paddle right away
	}

	//--------------------------CHECK FOR COLLISIONS-------------------------------//

	//check collision with right boundary
	if (collisionRect(ball.x, ball.y, ball.x + BALL_SIZE, ball.y + BALL_SIZE, GAME_WIDTH, 0, GAME_WIDTH + 10, GAME_HEIGHT))
	{
		ball.xspeed = -ball.xspeed;
		ball.x -= 2;
	}

	//check collision with top boundary
	if (collisionRect(ball.x, ball.y, ball.x + BALL_SIZE, ball.y + BALL_SIZE, 0, -10, GAME_WIDTH, 0))
	{
		ball.yspeed = -ball.yspeed;
		ball.y += 2;
	}

	//check collision with left boundary
	if (collisionRect(ball.x, ball.y, ball.x + BALL_SIZE, ball.y + BALL_SIZE, -10, 0, 0, GAME_HEIGHT))
	{
		ball.xspeed = -ball.xspeed;
		ball.x += 2;
	}

	//check collision with bricks
	for (int i = 0; i < NUM_OF_BRICKS; i++)
	{
		if (brickArray[i].health && collisionRect(ball.x, ball.y, ball.x + BALL_SIZE, ball.y + BALL_SIZE, brickArray[i].x, brickArray[i].y,
			brickArray[i].x + BRICK_WIDTH, brickArray[i].y + BRICK_HEIGHT))
		{
			int ballxmid = ball.x + BALL_SIZE/2;
			int ballymid = ball.y + BALL_SIZE/2;
			int brickxmid = brickArray[i].x + BRICK_WIDTH/2;
			int brickymid = brickArray[i].y + BRICK_HEIGHT/2;

			//slope of the vector pointing from the ball to the brick
			int deltax = brickxmid - ballxmid;
			int deltay = brickymid - ballymid;

			//below or above brick
			if (abs(deltax) < 2 * abs(deltay) + 2) // abs(dy/dx) > 1/2 			(visual->)  \_____/
			{
				ball.yspeed = -ball.yspeed;
				ball.y += signOf(ball.yspeed) * 2; //"push it out of brick just a bit"
			}
			//side of brick
			else
			{
				ball.xspeed = -ball.xspeed;
				ball.x += signOf(ball.xspeed) * 2; //"push it out of brick just a bit"
			}



			brickArray[i].health -= 1;

			if (!brickArray[i].health)
			{
				//draw part of the background image that was previously hidden
				drawImageExt3(brickArray[i].x, brickArray[i].y, brickArray[i].x, brickArray[i].y, BRICK_WIDTH, BRICK_HEIGHT, gtech);
				bricksLeft --;
   				sprintf(bricksString, "%d", bricksLeft);  		//store lives as a string

   				waitForVblank();
   				drawRect(215, 85, 12, 8, RGB(6, 0, 6));
    			drawString(215, 85, bricksString, WHITE);
			}
		}
	}

	//check collision with paddle
	if (!ball.onPaddle && collisionRect(ball.x, ball.y, ball.x + BALL_SIZE, ball.y + BALL_SIZE, paddle.x, PADDLE_Y, paddle.x + PADDLE_WIDTH, PADDLE_Y + PADDLE_HEIGHT))
	{
		setNewBallSpeed();
	}

	//check for death
	if (ball.y > GAME_HEIGHT)
	{
		lives--;
		sprintf(livesString, "%d", lives);  		//store lives as a string

		//draw new lives number
		waitForVblank();
		drawRect(215, 35, 6, 8, RGB(6, 0, 6));
    	drawString(215, 35, livesString, WHITE);

		setBallOnPaddle();
	}

	ball.x += ball.xspeed;
	ball.y += ball.yspeed;
}

void drawEverything()
{
	waitForVblank();

	//min functions for width prevent ball from drawing over the info panel
	//since the ball can go a bit off the screen before it changes directions
	repaintColorsUnderBall();
	drawRect(ball.x, ball.y, min(BALL_SIZE, GAME_WIDTH - ball.x), BALL_SIZE, BALL_COLOR);

	drawRect(oldPaddle.x, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, BLACK);
	drawRect(paddle.x, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_COLOR);
}

void setBallOnPaddle()
{
	ball.onPaddle = TRUE;
	ball.xspeed = 0;
	ball.yspeed = 0;
	ball.x = paddle.x + PADDLE_WIDTH / 2;
	ball.y = PADDLE_Y - BALL_SIZE;
}

//since GBA can't do double arithmetic, determining new velocities with
//sine and cosine isn't feasible. instead, this function identifies
//six possible regions in which the collision with the paddle can occur
//and sets the ball to an according hard-coded speed
void setNewBallSpeed()
{
	int ballMiddle = ball.x + BALL_SIZE / 2;

	//6 different speed possibilities
	//36 possible x positions for the middle of the ball
	//on collision. (3 pixels on either side of paddle, plus paddle)

	// (3 pixels to left) - - - [==PADDLE (30 pixels)==] - - - (3 pixels to right)

	//positions 0-5:
	if (ballMiddle < paddle.x + 3)
	{
		ball.xspeed = -3;
		ball.yspeed = -1;
	}

	//positions 6-11:
	if (ballMiddle >= paddle.x + 3 && ballMiddle < paddle.x + 9)
	{
		ball.xspeed = -2;
		ball.yspeed = -2;
	}

	//positions 12-17:
	if (ballMiddle >= paddle.x + 9 && ballMiddle < paddle.x + 15)
	{
		ball.xspeed = -1;
		ball.yspeed = -3;
	}

	//positions 18-23:
	if (ballMiddle >= paddle.x + 15 && ballMiddle < paddle.x + 21)
	{
		ball.xspeed = 1;
		ball.yspeed = -3;
	}

	//positions 24-29:
	if (ballMiddle >= paddle.x + 21 && ballMiddle < paddle.x + 27)
	{
		ball.xspeed = 2;
		ball.yspeed = -2;
	}

	//positions 30-35:
	if (ballMiddle >= paddle.x + 27)
	{
		ball.xspeed = 3;
		ball.yspeed = -1;
	}
}

//10 cols, 5 rows
void instantiateBrickArray()
{
	int colorChoice = 0;

	for (int i = 0; i < NUM_OF_BRICKS; i++)
	{
		brickArray[i].x = (i % 10) * BRICK_WIDTH;
		brickArray[i].y = (i / 10) * BRICK_HEIGHT;
		brickArray[i].health = 1;
	}

	for (int i = 0; i < NUM_OF_BRICKS; i++)
	{

		colorChoice = i % 6;

		if (brickArray[i].health)
		{
			switch(colorChoice)
			{
				case 0:
					drawImage3(brickArray[i].x, brickArray[i].y, BRICK_WIDTH, BRICK_HEIGHT, redbrick);
					break;

				case 1:
					drawImage3(brickArray[i].x, brickArray[i].y, BRICK_WIDTH, BRICK_HEIGHT, bluebrick);
					break;

				case 2:
					drawImage3(brickArray[i].x, brickArray[i].y, BRICK_WIDTH, BRICK_HEIGHT, greenbrick);
					break;

				case 3:
					drawImage3(brickArray[i].x, brickArray[i].y, BRICK_WIDTH, BRICK_HEIGHT, yellowbrick);
					break;

				case 4:
					drawImage3(brickArray[i].x, brickArray[i].y, BRICK_WIDTH, BRICK_HEIGHT, purplebrick);
					break;

				case 5:
					drawImage3(brickArray[i].x, brickArray[i].y, BRICK_WIDTH, BRICK_HEIGHT, whitebrick);
					break;
			}
		}
	}
}

void repaintColorsUnderBall()
{
	if (oldBall.y > 80)
	{
		drawRect(max(0, oldBall.x), oldBall.y, min(BALL_SIZE, GAME_WIDTH - oldBall.x), BALL_SIZE, BLACK);
	}
	else
	{
		drawImageExt3(max(0, oldBall.x), oldBall.y, max(0, oldBall.x), oldBall.y, min(BALL_SIZE, GAME_WIDTH - oldBall.x), min(BALL_SIZE, 80 - oldBall.y), gtech);

		//clean up part of ball that WASN't on the image
		if (80 - oldBall.y <= BALL_SIZE)
		{
			drawRect(max(0, oldBall.x), 80, min(BALL_SIZE, GAME_WIDTH - oldBall.x), BALL_SIZE - (80 - oldBall.y), BLACK);
		}
	}
}