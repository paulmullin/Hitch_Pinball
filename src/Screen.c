/*
 * Screen.c
 *
 *  Created on: Dec 27, 2017
 *      Author: paul mullin
 */

#include<stdlib.h>
#include<stdio.h>

typedef struct { int X; int Y; int Type; } PIXLOC;
extern PIXLOC Pixel_XYT[];
extern unsigned int pd[];

void Screen_Run(void);
void Screen_DrawBall(int x,int y,unsigned int color,int intensity,char isOr);

typedef struct  {
	int iX,iY;     //location
	int iDX,iDY;   //increments
	unsigned int uiColor;
	int iIntensity;
	int iSize;

	int i2X,i2Y;
	int iD2X,iD2Y;
	unsigned int ui2Color;
	int i2Intensity;
	int i2Size;


	int iTimer;
}ScreenType;


ScreenType Screen = { 100, 100, 4, 5, 0x00000001, 0x20, 3,   240,330, 0, -5, 0x00010000, 0x20, 3, 1 };



void Screen_Run(void)
{

		Screen_DrawBall(Screen.iX,Screen.iY,Screen.uiColor,Screen.iIntensity,0);
		Screen_DrawBall(Screen.i2X,Screen.i2Y,Screen.ui2Color,Screen.i2Intensity,1);

		Screen.i2Y-=5;
		Screen.i2X+=Screen.iD2X;

		Screen.iX+=Screen.iDX;
		Screen.iY+=Screen.iDY;

		if (Screen.iX>370 || Screen.iY>370 || Screen.iX<99 || Screen.iY<99) {

			Screen.iX=100;
			Screen.iY=240-70+rand()%100;
			Screen.iDX=rand()%4+1;
			Screen.iDY=rand()%4-2;

			Screen.i2X=240;
			Screen.i2Y=330;
			Screen.iD2X=rand()%3-1;
		}

}



/*
 * Draw a ball with shading on the screen
 *
 * x,y location in mm
 * color - use base P_ colors
 * intensity (basicaly size
 * isOr - true=OR all pixels, false=Write all pixels (even off)  use this to drw multipl overlapping objects
 *
 */
void Screen_DrawBall(int x,int y,unsigned int color,int intensity,char isOr)
{
	int iPixelNum;
	int iDistance,iResult;

		//for each pixel in screen, find distance, set intensity
		for (iPixelNum=0; iPixelNum<=255; iPixelNum++)
		{
			iDistance = ((Pixel_XYT[iPixelNum].X - x) * (Pixel_XYT[iPixelNum].X - x));
			iDistance += ((Pixel_XYT[iPixelNum].Y - y) * (Pixel_XYT[iPixelNum].Y - y));

			iResult=1;
			while ( (iResult*iResult) < iDistance ) iResult++;

			if ((intensity-iResult)>0)
			{
				if (isOr!=1) pd[iPixelNum]=color*(intensity-iResult);
				else pd[iPixelNum]|=color*(intensity-iResult);
			}
			else
			{
				if (isOr!=1) pd[iPixelNum]=0;
			}
		}
}

