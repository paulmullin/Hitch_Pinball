/*
 * Hitchhiker Pinball MAIN
 *
 * Loads up PRU 0 driver used for output to string of serial neopixels
 * PRU also provides timing (~25 frames per second), main loop runs at frame rate
 * as it is sync'd to PRU
 *
 *
*   http://processors.wiki.ti.com/index.php/PRU_Linux_Application_Loader_API_Guide
*/

#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>

//#include "__prussdrv.h"
#include <pruss_intc_mapping.h>
#include <time.h>

#define PRU_NUMBER 0

extern void LifePixels_Run (void);
extern void LeftPixelsRun(void);
extern void RocketPixelsRun(void);
extern void BonusPixels_Run(void);
extern void Screen_Run(void);
extern void GlobePixels_Init(void);
extern void GlobePixels_Run(void);
extern void MultiPixelsRun(void);

//Pixel Data
unsigned int pd[400],pHeader[6]={0,5,1,1};


volatile int32_t* init_prumem()
  {
  	volatile int32_t* p;
  	prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, (void**)&p);
  	return p;
  }




int main (void)
{
int x;
volatile int32_t *uiTimerPtr=(volatile int32_t *)0xB6D30000;
int32_t uiLocalTimer=0;



//unsigned int *prutimer = (unsigned int *) prussdrv.pru0_dataram_base;
int32_t uiTimer,t;


   //init pixel array data ("pd") to OFF
   for(x=0; x<399; x++) pd[x]=0x00000000;

   // Initialize structure used by prussdrv_pruintc_intc
   // PRUSS_INTC_INITDATA is found in pruss_intc_mapping.h
   tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

   // Allocate and initialize memory
   prussdrv_init ();
   prussdrv_open (PRU_EVTOUT_0);

   // Map PRU's interrupts
   prussdrv_pruintc_init(&pruss_intc_initdata);

   prussdrv_pru_write_memory(PRUSS0_PRU0_DATARAM,0,pHeader,4*4);
   uiTimer = *uiTimerPtr;

   prussdrv_pru_write_memory(PRUSS0_PRU0_DATARAM,4,pd,4*399); //load intial data

   // Load and execute the PRU program on the PRU
   prussdrv_exec_program (PRU_NUMBER, "./pixel_driver.bin");

   //call all application required initializations
   GlobePixels_Init();

   volatile int32_t * pruDataMem0 = init_prumem();

   while(1)
   {
	   LifePixels_Run();
	   RocketPixelsRun();
	   BonusPixels_Run();
	   Screen_Run();
	   GlobePixels_Run();
	   MultiPixelsRun();
	   LeftPixelsRun();

	   // pd[326]=0x00208000;  //yellow=0x00010100, purple=0x00000101, teal=0x00C000C0  green:red:blue

	   //copy data to PRU memory
	   prussdrv_pru_write_memory(PRUSS0_PRU0_DATARAM,4,pd,4*399);

	   uiTimer = *uiTimerPtr;

	   printf("timer %u \n",pruDataMem0[0]);

	   while(uiLocalTimer==pruDataMem0[0]) {} //wait for PRU "tick"

	   uiLocalTimer++;
   }


   // Disable PRU and close memory mappings
   prussdrv_pru_disable(PRU_NUMBER);
   prussdrv_exit ();

   return EXIT_SUCCESS;
}
