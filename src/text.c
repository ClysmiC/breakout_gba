#include "text.h"
#include "mylib.h"

void drawChar(int x, int y, char ch, unsigned short color)
{
	for(int down = 0; down < 8; down++)
	{
		for(int across = 0; across < 6; across++)
		{
			if(fontdata_6x8[OFFSET(across, down, 6)+ch*48])
			{
				setPixel(x + across, y + down, color);
			}
		}
	}
}

/*
 *
 * I didn't code this in class but I will go over it next lecture
 *
 * You can call it like this:
 *
 * drawString(10, 10, "Hello!", YELLOW);
 *
 * or like this:
 * 
 * char *message = "This is a message.";
 * drawString(20, 5, message, RED);
 *
 * or like this:
 *
 * char buffer[41] = "This is a char array!";
 * drawString(30, 5, buffer, GREEN);
 * 
 */
void drawString(int x, int y, char *str, unsigned short color)
{
	while(*str)
	{
		drawChar(x, y, *str++, color);
		x +=6;

	}
}
