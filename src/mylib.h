typedef unsigned short u16;

//getting pixels on screen utilities

#define REG_DISPCNT     ((u16*)0x4000000)
#define MODE3 			3
#define BG2_ENABLE 		(1 << 10)
#define videoBuffer     ((u16*)0x6000000)
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   160
#define GAME_WIDTH      200   //part of screen that the actual gameplay occupies
#define GAME_HEIGHT     160
#define OFFSET(x, y, width) 	(((y) * (width)) + (x))


//drawing on screen

void setPixel(int, int, u16);
void drawRect(int, int, int, int, u16);
void drawHollowRect(int, int, int, int, u16);
void drawStar(int, int, u16);
void plotLine(int, int, int, int, u16);
int calculateOctant(int, int);
int xPrime(int, int, int);
int yPrime(int, int, int);
void setPixelInOctant(int, int, int, u16);
u16 getColor(int, int);
int getNumberOfSetPixels();

void fillScreen(volatile u16);
void drawBackground3(const u16 *image);
void drawImage3 (int x, int y, int w, int h, const u16* image);
void drawImageExt3(int x, int y, int imagex, int imagey, int w, int h, const volatile u16 *image);

//colors

#define RGB(r, g, b) 	((b << 10) | (g << 5) | (r))
#define RED             RGB(31,0,0)
#define GREEN           RGB(0, 31,0)
#define BLUE            RGB(0, 0, 31)
#define CYAN            RGB(0, 31, 31)
#define MAGENTA         RGB(31, 0, 31)
#define YELLOW          RGB(31, 31, 0)
#define WHITE           RGB(31, 31, 31)
#define BLACK  			0

#define FLAPPY_COLOR        RGB(20, 0, 20)
#define SKY_COLOR           RGB(14,25,26)
#define FRAME_COLOR         0x02464


//input

#define BUTTON_A		(1<<0)
#define BUTTON_B		(1<<1)
#define BUTTON_SELECT	(1<<2)
#define BUTTON_START	(1<<3)
#define BUTTON_RIGHT	(1<<4)
#define BUTTON_LEFT		(1<<5)
#define BUTTON_UP		(1<<6)
#define BUTTON_DOWN		(1<<7)
#define BUTTON_R		(1<<8)
#define BUTTON_L		(1<<9)

#define BUTTONS (*(unsigned int *)0x4000130)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)
#define ANY_KEY_DOWN (KEY_DOWN_NOW(BUTTON_A) || KEY_DOWN_NOW(BUTTON_B) || KEY_DOWN_NOW(BUTTON_SELECT) || KEY_DOWN_NOW(BUTTON_START) || KEY_DOWN_NOW(BUTTON_RIGHT) || KEY_DOWN_NOW(BUTTON_LEFT) ||KEY_DOWN_NOW(BUTTON_UP) || KEY_DOWN_NOW(BUTTON_DOWN) ||KEY_DOWN_NOW(BUTTON_L) || KEY_DOWN_NOW(BUTTON_R))


//drawing without tearing

#define SCANLINECOUNTER *(volatile unsigned short *)0x4000006
void waitForVblank();


//DMA
typedef struct
{
	const volatile void *src;
	const volatile void *dst;
	unsigned int cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

// Defines
#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)
#define DMA_DESTINATION_RESET (3 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)


//utility

#define TRUE 	1
#define FALSE	0

void delay(int);
int min(int, int);
int max(int, int);
int abs(int);
int signOf(int);
int collisionRect(int x1, int y1, int x2, int y2, int xx1, int yy1, int xx2, int yy2); //check 2 rectangles to see if they collide
int isInBounds(int x, int y, int w, int h);