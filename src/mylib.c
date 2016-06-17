#include "mylib.h"

void setPixel(int x, int y, u16 color)
{
    //out of screen bounds
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT || x < 0 || y < 0)
        return;

    videoBuffer[OFFSET(x,y,SCREEN_WIDTH)] = color;
}

u16 getColor(int x, int y)
{
    return videoBuffer[OFFSET(x,y,SCREEN_WIDTH)];
}

int getNumberOfSetPixels()
{
    int pixelCounter = 0;

    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            if (getColor(x, y) != 0) //pixel isn't black
            {
                pixelCounter++;
            }
        }
    }

    return pixelCounter;
}

void drawRect(int x, int y, int width, int height, u16 color)
{
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            setPixel(x + col, y + row, color);
        }
    }
}

void drawHollowRect(int x, int y, int width, int height, u16 color)
{
    for (int col = 0; col < width; col++)
    {
        //top side
        setPixel(x + col, y, color);

        //bottom side
        setPixel(x + col, y + height - 1, color);
    }

    for (int row = 0; row < height; row++)
    {
        //left side
        setPixel(x, y + row, color);

        //right side
        setPixel(x + width - 1, y + row, color);
    }
}

//draw a star that fits inside a 40x40 rectangle positioned
//at the point (x, y)
void drawStar(int x, int y, u16 color)
{
    plotLine(x + 9, y + 37, x + 19, y + 3, color);
    plotLine(x + 19, y + 3, x + 30, y + 37, color);
    plotLine(x + 30, y + 37, x + 3, y + 16, color);
    plotLine(x + 3, y + 16, x + 37, y + 16, color);
    plotLine(x + 37, y + 16, x + 9, y + 37, color);
}


/**
 *  Octants
 *   \2|1/
 *   3\|/0
 *  ---+---
 *   4/|\7
 *   /5|6\
 *
 *
 */
void plotLine(int x0, int y0, int x1, int y1, u16 color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int octant = calculateOctant(dx, dy);

    int x0Copy = x0;
    int y0Copy = y0;
    int x1Copy = x1;
    int y1Copy = y1;

    //change inputs to make the calculations look like they are in
    //octant 7 (the one that the psuedocode works for)
    x0 = xPrime(x0Copy, y0Copy, octant);
    x1 = xPrime(x1Copy, y1Copy, octant);
    y0 = yPrime(x0Copy, y0Copy, octant);
    y1 = yPrime(x1Copy, y1Copy, octant);
    
    //recalculate dx and dy for the new x and y's
    dx = x1 - x0;
    dy = y1 - y0;

    int D = (dy << 1) - dx;         //D = 2*dy - dx
    setPixelInOctant(x0, y0, octant, color);
    int y = y0;

    for (int x = x0 + 1; x <= x1; x++)
    {
        if (D > 0)
        {
            y++;
            setPixelInOctant(x, y, octant, color);
            D = D + ((dy << 1) - (dx << 1));    //D = D + (2*dy - 2*dx)
        }
        else
        {
            setPixelInOctant(x, y, octant, color);
            D = D + (dy << 1);                  //D = D + 2*dy
        }
    }
}

/**
 * Given dx and dy, returns which octant the
 * line is drawn in.
 *
 *   \2|1/
 *   3\|/0
 *  ---+---
 *   4/|\7
 *   /5|6\
 */
int calculateOctant(int dx, int dy)
{
    if (dx >= 0 && dy >= 0)             //octants 6/7
    {
        if (dx - dy >= 0)
        {
            return 7;
        }
        else
        {
            return 6;
        }
    }
    else if (dx >= 0 && dy < 0)         //octants 0/1
    {
        if (dx + dy >= 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else if (dx < 0 && dy < 0)         //octants 2/3
    {
        if (dx - dy < 0)
        {
            return 3;
        }
        else
        {
            return 2;
        }
    }
    else if (dx < 0 && dy >= 0)         //octants 4/5
    {
        if (dx + dy < 0)
        {
            return 4;
        }
        else
        {
            return 5;
        }
    }

    return -1; //error
}

/**
 * Given (x, y) and the line is being drawn in, return the
 * corresponding x that will make the calculations work
 * as if the line were being drawn in octant 7
 */
int xPrime(int x, int y, int octant)
{
    switch(octant)
    {
        case 1:
        case 2:
            return 0 - y;

        case 3:
        case 4:
            return 0 - x;

        case 5:
        case 6:
            return y;

        case 0:
        case 7:
        default:
            return x;
    }

}

/**
 * Given (x, y) and the line is being drawn in, return the
 * corresponding y that will make the calculations work
 * as if the line were being drawn in octant 7
 */
int yPrime(int x, int y, int octant)
{
    switch(octant)
    {
        case 0:
        case 3:
            return 0 - y;

        case 1:
        case 6:
            return x;

        case 2:
        case 5:
            return 0 - x;

        case 4:
        case 7:
        default:
            return y;
    }
}

/**
 * Given the transformed x and y (from xPrime() and yPrime()) and the
 * octant the line is being drawn in, set the correct pixel.
 */
void setPixelInOctant(int x, int y, int octant, u16 color)
{
    switch(octant)
    {
        case 0:
            setPixel(x, 0 - y, color);
            break;

        case 1:
            setPixel(y, 0 - x, color);
            break;

        case 2:
            setPixel(0 - y, 0 - x, color);
            break;

        case 3:
            setPixel(0 - x, 0 - y, color);
            break;

        case 4:
            setPixel(0 - x, y, color);
            break;

        case 5:
            setPixel(0 - y, x, color);
            break;

        case 6:
            setPixel(y, x, color);
            break;

        case 7:
        default:
            setPixel(x, y, color);
            break;
    }
}

void delay(int t)
{
    volatile int x;

    for (int i = 0; i < t * 10000; i++)
    {
        x = i;
    }

    return;
}

void waitForVblank()
{
    while(SCANLINECOUNTER>160);
    while(SCANLINECOUNTER<160);
    return;
}

int min(int one, int two)
{
    if (one <= two)
        return one;

    return two;
}

int max(int one, int two)
{
    if (one >= two)
        return one;

    return two;
}

int abs(int x)
{
    if (x < 0)
        return -x;

    return x;
}

int signOf(int x)
{
    if (x < 0)
        return -1;

    return 1;
}

//check 2 rectangles to see if they collide
int collisionRect(int x1, int y1, int x2, int y2, int xx1, int yy1, int xx2, int yy2)
{
    //check top left point of rect1
    if (x1 >= xx1 && x1 <= xx2 && y1 >= yy1 && y1 <= yy2)
        return 1;

    //check top right point of rect1
    if (x2 >= xx1 && x2 <= xx2 && y1 >= yy1 && y1 <= yy2)
        return 1;

    //check bottom left point of rect1
    if (x1 >= xx1 && x1 <= xx2 && y2 >= yy1 && y2 <= yy2)
        return 1;

    //check bottom right point of rect1
    if (x2 >= xx1 && x2 <= xx2 && y2 >= yy1 && y2 <= yy2)
        return 1;

    return 0;
}

//check if in-bounds
int isInBounds(int x, int y, int width, int height)
{
    if (x < 0 || x + width > GAME_WIDTH)
        return 0;

    if (y < 0 || y + height > GAME_HEIGHT)
        return 0;

    return 1;
}

/*****DMA STUFF******/
void fillScreen(volatile u16 color)
{
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = (SCREEN_HEIGHT * SCREEN_WIDTH) | DMA_ON | DMA_SOURCE_FIXED | DMA_DESTINATION_INCREMENT;
}

//MUST PASS A POINTER TO AN ARRAY W/ DATA FROM A 240x160 IMAGE
void drawBackground3(const u16 *image)
{
    DMA[3].src = image;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = (SCREEN_HEIGHT * SCREEN_WIDTH) | DMA_ON | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT;
}

void drawImage3(int x, int y, int w, int h, const u16 *image)
{
    for (int row = 0; row < h; row++)
    {
        DMA[3].src = image + OFFSET(0, row, w);
        DMA[3].dst = videoBuffer + OFFSET(x, y + row, SCREEN_WIDTH);
        DMA[3].cnt = w | DMA_ON | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT;
    }
}

//draws only the selection from imagex -> imagex + w (and likewise for height)
//at the position x, y
void drawImageExt3(int x, int y, int imagex, int imagey, int w, int h, const volatile u16 *image)
{
    for (int row = 0; row < h; row++)
    {
        DMA[3].src = image + OFFSET(imagex, imagey + row, 200);
        DMA[3].dst = videoBuffer + OFFSET(x, y + row, SCREEN_WIDTH);
        DMA[3].cnt = w | DMA_ON | DMA_SOURCE_INCREMENT | DMA_DESTINATION_INCREMENT;
    }
}