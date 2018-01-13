/*
 * Pixel_Controls.c
 *
 *  Created on: Dec 27, 2017
 *      Author: paul mullin
 *
 *  Various feature pixel control code
 */
#include <stdlib.h>
#include <stdio.h>
#include <Pixel_Layout.h>

extern unsigned int pd[]; //memory space for actual pixel data



void LifePixels_Run (void);
void Pixel_ShadeDist(int x,int y,unsigned int color,int intensity,char isOr,int *iPixelList);



/*
 * Life Piel control
 *
 * life, universe, everything, extra everything, rainbow, beatelguse and earth
 *
 *
 */
typedef struct {
	int iState;
	int iTimer;
}LifePixelsType;

LifePixelsType LifePixels = { 0, 0};
int LifePixelsAddr[10]= {LIFE, UNIVERSE, EVERYTHING, EVERYTHING_LEFT, BETELGUESE, RAINBOW };

void LifePixels_Run (void)
{
	pd[LIFE]=P_TEAL * 0x50;
	pd[UNIVERSE]=P_PURPLE * 0x50;
	pd[EVERYTHING]=P_YELLOW * 0x40;

	pd[EVERYTHING_LEFT]=P_WHITE;
	pd[BETELGUESE]=P_OFF;
	pd[RAINBOW]=P_YELLOW * 30;

}


/*
 *
 * left side and drain pixel driver
 */
typedef struct {
	int iState;
	int iTimer;
	int iPixelIndex1;
	int iPixelIndex2;
	int iColor;
	int icolorIndex;

}LeftPixelType;

LeftPixelType LeftPixels = {0,0};
int LeftPixelAddr[15]={ DOLPIN, MOUSE, LEFTRUN_TR, LEFTRUN_TL, LEFTRUN_CR, LEFTRUN_CL, LEFTRUN_LR, DRAIN_LEFT, DRAIN_CENTER, -1 };
int StartingColors[8] = { 0x00000001, 0x00010000, 0x00000100, 0x00010101, 0x00000101 };

#define LPNUMBER_COLORS 5
#define LPNUMBER_PIXELS 9
#define LPSTATE_OFF 0
#define LPSTATE_INCREASE 1
#define LPSTATE_DECREASE 2


void LeftPixelsRun(void)
{
int i;

	switch (LeftPixels.iState)
	{
	case LPSTATE_OFF:
		for (i=0; i<LPNUMBER_PIXELS; i++) pd[LeftPixelAddr[i]]=0;
		if (--LeftPixels.iTimer<=0)
		{
			LeftPixels.iPixelIndex1=rand()%LPNUMBER_PIXELS;
			LeftPixels.iPixelIndex2=rand()%LPNUMBER_PIXELS;
			LeftPixels.icolorIndex=rand()%LPNUMBER_COLORS;
			LeftPixels.iColor=StartingColors[LeftPixels.icolorIndex];
			LeftPixels.iTimer = 0x70;
			LeftPixels.iState=LPSTATE_INCREASE;
		}
	break;
	case LPSTATE_INCREASE:
		pd[LeftPixelAddr[LeftPixels.iPixelIndex1]]=LeftPixels.iColor;
		pd[LeftPixelAddr[LeftPixels.iPixelIndex2]]=LeftPixels.iColor;

		pd[COMET_2]=LeftPixels.iColor;
		pd[COMET_3]=LeftPixels.iColor;

		LeftPixels.iColor += StartingColors[LeftPixels.icolorIndex];
		if (--LeftPixels.iTimer <=0)
			{
			LeftPixels.iState=LPSTATE_DECREASE;
			LeftPixels.iTimer = 0x70;
			}
	break;
	case LPSTATE_DECREASE:
		pd[LeftPixelAddr[LeftPixels.iPixelIndex1]]=LeftPixels.iColor;
		pd[LeftPixelAddr[LeftPixels.iPixelIndex2]]=LeftPixels.iColor;

		pd[COMET_2]=LeftPixels.iColor;
		pd[COMET_3]=LeftPixels.iColor;

		LeftPixels.iColor -= StartingColors[LeftPixels.icolorIndex];
		if (--LeftPixels.iTimer <=0)
			{
				LeftPixels.iState=LPSTATE_OFF;
				LeftPixels.iTimer = 4;
			}
	break;
	default:
		LeftPixels.iState=LPSTATE_OFF;
	break;
	}
}



/*
 * Rocket pixel driver
 *
 */
typedef struct {
		int iState;
		int iTimer;
		unsigned int uiAccel;
}RocketPixelsType;

#define RSTATE_BLANK 0
#define RSTATE_BLUE 1
#define RSTATE_YELLOW 2
#define RSTATE_FIRE 4

int RocketPixelAddr[9]={ROCKET7, ROCKET6, ROCKET5, ROCKET4, ROCKET3, ROCKET2, ROCKET1, -1};

RocketPixelsType RocketPixels = { 0, 20, 3 };

void RocketPixelsRun(void)
{
int i;

		switch (RocketPixels.iState)
		{
			case RSTATE_BLANK:
				for (i=0; i<7; i++) pd[RocketPixelAddr[i]]=0;
				if (--RocketPixels.iTimer<=0)
					{
						RocketPixels.iState=RSTATE_BLUE;
					}
			break;

			case RSTATE_BLUE:
				i=0;
				while (pd[RocketPixelAddr[i]]>=40 && i<8) i++;

				if (i<7) {
					pd[RocketPixelAddr[i]]+=RocketPixels.uiAccel;
				}
				else {
					RocketPixels.uiAccel+=5;
					RocketPixels.iState=RSTATE_BLANK;
					RocketPixels.iTimer=60-RocketPixels.uiAccel;
					if (RocketPixels.uiAccel>=80) {
						RocketPixels.iState=RSTATE_YELLOW;
						RocketPixels.uiAccel=0x00505000;
						RocketPixels.iTimer=150;
					}
				}
			break;

			case RSTATE_YELLOW:
				for (i=3; i<7; i++) pd[RocketPixelAddr[i]]=pd[RocketPixelAddr[0]];
				pd[RocketPixelAddr[3+rand()%4]] = RocketPixels.uiAccel; //color change - growing red
				if ((RocketPixels.uiAccel& 0x0000FF00) < 0x0000D000) RocketPixels.uiAccel+=0x00000100; //more red
				if ((RocketPixels.uiAccel& 0x00FF0000) > 0x00000000) RocketPixels.uiAccel-=0x00010000; //less green
				if (--RocketPixels.iTimer <= 0)
				{
					RocketPixels.iState=RSTATE_FIRE;
					RocketPixels.iTimer=150;
				}
			break;

			case RSTATE_FIRE:
				pd[RocketPixelAddr[3]]=pd[RocketPixelAddr[0]];
				for (i=4; i<7; i++) pd[RocketPixelAddr[i]]=0x0000DD00;  //RED
				pd[RocketPixelAddr[4+rand()%3]] = 0x00505000;
				pd[RocketPixelAddr[4+rand()%3]] = 0x00208000; //orange
				if (--RocketPixels.iTimer <= 0) { RocketPixels.iState=RSTATE_BLANK; RocketPixels.uiAccel=3; RocketPixels.iTimer=90; }
			break;

			default:
				RocketPixels.iState=RSTATE_BLANK;
			break;
		}

}



/*
 * Multiplier row -
 *
 *
 */
typedef struct {
		int iY;
}MultiPixelsType;

int MultiPixelAddr[7]={MULTx1, MULTx2, MULTx3, MULTx4, MULTx5, -1};

MultiPixelsType MultiPixels = { 400 };

void MultiPixelsRun(void)
{
	MultiPixels.iY+=4;
	if (MultiPixels.iY>900) MultiPixels.iY=400;

	Pixel_ShadeDist(-1,MultiPixels.iY,0x00000100,0xA0,0,MultiPixelAddr);
}



/*
 * Bonus Pixels (100-900 point bonus series)
 *
 *
 */
int BonusPixelAddr[10]={BONUS100,BONUS200,BONUS300,BONUS400,BONUS500,BONUS600,BONUS700,BONUS800,BONUS900,-1};
#define BonusPixelcount 9
#define BrightMulti 2

typedef struct  {
	unsigned int iColor1,iColor2;
	int iY;
	int iYDelta;
	int iTimer;
}BonusPixelsType;

BonusPixelsType BonusPixels = { 0x00000101, 0x00000001, 240, 10, 10 };

//Bonus Pixels Run Function
void BonusPixels_Run(void)
{
    //run up and down, 2 color
	BonusPixels.iY+=BonusPixels.iYDelta;

	if (BonusPixels.iY>=800 || BonusPixels.iY<=240)
	{
		BonusPixels.iYDelta*=-1;
	}

	Pixel_ShadeDist(-1,BonusPixels.iY,BonusPixels.iColor1,0x40,0,BonusPixelAddr);
	Pixel_ShadeDist(-1,800+240-BonusPixels.iY,BonusPixels.iColor2,0x40,1,BonusPixelAddr);
}





/*
 * Globe Pixel driver
 *
 * center of globe is here-> (380, 150)
 */
typedef struct  {
	unsigned int iColor1;
	int iX,iY;
	int iXindex,iYindex;
	int iTimer;
}BonusGlobeType;

int iQCircle[38]= { 30,28,26,23,19,15,10,5,0,-5,-10,-15,-19,-23,-26,-28,-30,-30,-30, -28,-26,-23,-19,-15,-10,-5,0,5,10,15,19,23,26,28,30,30 };
int GlobePixelAddr[30]; //={GLOBE_START,GLOBE_START+1,GLOBE_START+2
BonusGlobeType GlobePixels = { 0x00010100, 30, 0, 0, 8, 10};

#define FLASH_COLOR P_WHITE * 0x20  //0x00202020

/*
 *
 * Globe Pixels Initialization
 *
 */
void GlobePixels_Init(void)
{
	int i,q=0;

	//setup pixel list
	for (i=GLOBE_START; i<=GLOBE_END; i++)
	{
		GlobePixelAddr[q]=i;
		q++;
	}
	GlobePixelAddr[q]=-1;
}

/*
 *
 * globe plastics pixel run
 *
 */
void GlobePixels_Run(void)
{
		 //move to next point on circle
		 if (GlobePixels.iXindex>=35) GlobePixels.iXindex=0;
		 else GlobePixels.iXindex++;

		 if (GlobePixels.iYindex>=35) GlobePixels.iYindex=0;
		 else GlobePixels.iYindex++;

		 Pixel_ShadeDist( 380+iQCircle[GlobePixels.iXindex], 150+iQCircle[GlobePixels.iYindex],GlobePixels.iColor1,0x14,0,GlobePixelAddr);

		 if (--GlobePixels.iTimer<=0)
		 {
			 //flash
			 pd[GLOBE_START+10]=FLASH_COLOR;
			 pd[GLOBE_START+9]=FLASH_COLOR;
			 pd[GLOBE_START+11]=FLASH_COLOR;
			 pd[GLOBE_START+15]=FLASH_COLOR;
			 pd[GLOBE_START+5]=FLASH_COLOR;

			 GlobePixels.iTimer=rand()%100;
		 }
}




/*
 * Helper function, used by many above
 *
 * shade a list of pixels based on distance from x,y
 * single color
 *
 * set y or x to -1 to ignore that axis
 * with both x and y this functioncal actual distance
 */
void Pixel_ShadeDist(int x,int y,unsigned int color,int intensity,char isOr,int *iPixelList)
{
	int i=0;
	int iDistance,iResult,iMult;

		//for each pixel in set, find distance, set intensity
		while (iPixelList[i]>=0)
		{
			if (x>0) iDistance = ((Pixel_XYT[iPixelList[i]].X - x) * (Pixel_XYT[iPixelList[i]].X - x));
			else iDistance=0;

			if (y>0) iDistance += ((Pixel_XYT[iPixelList[i]].Y - y) * (Pixel_XYT[iPixelList[i]].Y - y));

			iResult=1;
			while ( (iResult*iResult) < iDistance ) iResult++;

			iMult = intensity-iResult;
			if (iMult>0)
			{
				if (iMult>0xFA) iMult=0xFA;
				if (isOr!=1) pd[iPixelList[i]]=color*iMult;
				else pd[iPixelList[i]]|=color*iMult;
			}
			else
			{
				if (isOr!=1) pd[iPixelList[i]]=0x00000000;
			}
			i++;
		}
}

