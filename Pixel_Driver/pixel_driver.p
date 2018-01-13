// PRU Program to use P9_27 (pru0_pru_r30_5) to send data to string
// of neopixels.  read data from shared memory one pixel per int (24 bits)

//Paul Mullin

.origin 0                        // start of program in PRU memory
.entrypoint START                // program entry point (for a debugger)
                           
#define LONG   80  //long pulse duration 0.8uS
#define SHORT  40  //short pulse duration 0.4uS
#define PAUSE  100*1000*30   //100*50 //50uS add 20ms for frame rate control

#define NUMBER_OF_PIXELS 350

//register usage
#define r0_Delay_Count r0
#define r1_Address r1
#define r2_Pixel_Data r2
#define r3_Address r3
#define r4_Pixel_Counter r4

#define r6_bit_pos r6


START:
	//number of pixels is outer loop
	MOV 	r4_Pixel_Counter,NUMBER_OF_PIXELS	
	MOV 	r1_Address,0x0010  //load start address of pixel data
	
NEXTLED:
	//low byte =BLUE,  middle = RED,  high byte= GREEN
	
	LBBO 	r2_Pixel_Data, r1_Address, 0, 4 //get data for this led
	MOV 	r6_bit_pos, 24  		//23rd bit starting from 0

SENDBIT:	
	SUB	r6_bit_pos,r6_bit_pos,1
	QBBS	ONE, r2_Pixel_Data ,r6_bit_pos    //brach to one if bit set



ZEROBIT:  //send zero bit
	SET	r30.t5           	// turn on the output pin (LED on)
	MOV	r0_Delay_Count, SHORT   // store the length of the delay in REG0
DELAYON:
	SUB	r0_Delay_Count, r0_Delay_Count, 1        // Decrement REG0 by 1
	QBNE	DELAYON, r0_Delay_Count, 0   		// Loop to DELAYON, unless REG0=0

	CLR	r30.t5           	// clear the output bin (LED off)
	MOV	r0_Delay_Count, LONG    // Reset REG0 to the length of the delay
DELAYOFF:
	SUB	r0_Delay_Count, r0_Delay_Count, 1        // decrement REG0 by 1
	QBNE	DELAYOFF, r0_Delay_Count, 0  		// Loop to DELAYOFF, unless REG0=0

	QBA	BITDONE



ONE:  	//send one bit
	SET	r30.t5           // turn on the output pin (LED on)
	MOV	r0_Delay_Count, LONG        // store the length of the delay in REG0
DELAYON1:
	SUB	r0_Delay_Count, r0_Delay_Count, 1        // Decrement REG0 by 1
	QBNE	DELAYON1, r0_Delay_Count, 0   // Loop to DELAYON, unless REG0=0

	CLR	r30.t5           // clear the output bin (LED off)
	MOV	r0_Delay_Count, SHORT        // Reset REG0 to the length of the delay
DELAYOFF1:
	SUB	r0_Delay_Count, r0_Delay_Count, 1        // decrement REG0 by 1
	QBNE	DELAYOFF1, r0_Delay_Count, 0  // Loop to DELAYOFF, unless REG0=0



BITDONE:
	QBNE	SENDBIT, r6_bit_pos, 0 //last bit for this pixel?

	ADD 	r1_Address, r1_Address, 4   	//next pixel data memory location
	SUB	r4_Pixel_Counter, r4_Pixel_Counter ,1   //number of leds left to send
	QBNE	NEXTLED, r4_Pixel_Counter, 0

	//all pixels sent, now delay 50uS - 20mS to lower frame rate to reasonable
	MOV	r0_Delay_Count, PAUSE
DELAYPAUSE:
	SUB 	r0_Delay_Count, r0_Delay_Count, 1
	QBNE	DELAYPAUSE, r0_Delay_Count, 0


	//timer output here (location 0)
	MOV 	r1_Address,0x0000   //load address of timer value
	LBBO 	r2_Pixel_Data, r1_Address, 0, 4 //get data (timer value)
	ADD	r2_Pixel_Data, r2_Pixel_Data, 1
	SBBO	&r2_Pixel_Data, r1_Address,0,4

	
	QBA 	START	


END:    // notify the calling app that finished
//	MOV	R31.b0, PRU0_R31_VEC_VALID | PRU_EVTOUT_0
//	HALT                     // halt the pru program



