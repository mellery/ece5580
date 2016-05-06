#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "main.h"

#include "stm32f4xx_it.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"

//#include "led-bytes.h"

// Private variables
volatile uint32_t time_var1, time_var2, time_var3;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

// Private function prototypes
void Delay(volatile uint32_t nCount);
void init();
void calculation_test();


int main(void) {
	init();

/*RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
TIM_TimeBaseInitTypeDef timerInitStructure; 
timerInitStructure.TIM_Prescaler = 40000;
timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
timerInitStructure.TIM_Period = 500;
timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
timerInitStructure.TIM_RepetitionCounter = 0;
TIM_TimeBaseInit(TIM2, &timerInitStructure);
TIM_Cmd(TIM2, ENABLE);
*/

	/*
	 * Disable STDOUT buffering. Otherwise nothing will be printed
	 * before a newline character or when the buffer is flushed.
	 */
	setbuf(stdout, NULL);

	calculation_test();

	for(;;) {
		GPIO_SetBits(GPIOD, GPIO_Pin_12);
		Delay(500);
		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		Delay(500);
	}

	return 0;
}

void calculation_test() {
	int iteration = 0;

	unsigned char p[8];
	unsigned char c[8];
	unsigned char k[16];

        unsigned short p16[4];
        unsigned short c16[4];
        unsigned short k16[8];
	
	unsigned short p1col[4];
	unsigned short c1col[4];
	unsigned short k1col[8];

	int kbits = 64;
	int i;

	 //BuildTableSCShRMCS();


	for(;;) {
		//GPIO_SetBits(GPIOD, GPIO_Pin_12);
		//Delay(500);
		//GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		//Delay(500);

		for(i = 0; i < 8; i++) c[i] = p[i] = 0x00; //& 0xff;
		for(i = 0; i < 16; i++)k[i] = 0x00; //rand() & 0xff;

		for (i=0; i<=3; i++)
		        p16[i]=c16[i]=0;
		for (i=0; i<=7; i++)
			k16[i]=0;	

	        c1col[0] = p1col[0] = k1col[0] = k1col[4] = 0x0000;
	        c1col[1] = p1col[1] = k1col[1] = k1col[5] = 0x0000;
	        c1col[2] = p1col[2] = k1col[2] = k1col[6] = 0x0000;
	        c1col[3] = p1col[3] = k1col[3] = k1col[7] = 0x0000;

/*		uint64_t opt_state;
		printf("optimized version\n");
		time_var2 = TIM_GetCounter(TIM2);
		//GPIO_SetBits(GPIOD, GPIO_Pin_12);
		opt_state = LEDRound(0, 0);
		//GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		time_var3 = TIM_GetCounter(TIM2) - time_var2;
		printf("Time:      %lu us\n\r", time_var3);
		printf("C = ");
		//for (i=0; i<8;i++)
		//	printf("%02x", opt_state[i]);
			//printf("%02x", c[i]);
                printf("%Lu", opt_state);
		printf("\n\n\r");

                printf("table version\n");
		time_var2 = TIM_GetCounter(TIM2);
                LED64table_core(p,k,c);
		time_var3 = TIM_GetCounter(TIM2) - time_var2;
		printf("Time:      %lu us\n\r", time_var3);
		printf("C = ");
		for (i=0; i<8;i++)
			printf("%02x", c[i]);
		printf("\n\n\r");
*/
/*		printf("origional version\n");
                time_var2 = TIM_GetCounter(TIM2);
  	        LED_enc(c, k, kbits);
                time_var3 = TIM_GetCounter(TIM2) - time_var2;
	        printf("Time:      %lu us\n\r", time_var3);
		printf("C = ");
		for (i=0; i<8;i++)
			printf("%02x", c[i]);
		printf("\n\n\r");
*/
/*		printf("byte version\n");
		time_var2 = TIM_GetCounter(TIM2);
		LED_enc_2col(c16, k16, kbits);
		time_var3 = TIM_GetCounter(TIM2) - time_var2;
		printf("Time:      %lu us\n\r", time_var3);
		printf("C = ");
		for (i=0; i<8;i++)
			printf("%02x", c[i]);
		printf("\n\n\r");
*/
/*		printf("tbox 4x2 version\n");
		time_var2 = TIM_GetCounter(TIM2);
		LED_enc_2col_tbox(c16, k16, kbits);
		time_var3 = TIM_GetCounter(TIM2) - time_var2;
		printf("Time:      %lu us\n\r", time_var3);
		printf("C = ");
		for (i=0; i<8;i++)
			printf("%02x", c[i]);
		printf("\n\n\r");
*/
/*		printf("two byte 2x2 version\n");
		time_var2 = TIM_GetCounter(TIM2);
		LED_enc_1col22(c16, k16, kbits);
		time_var3 = TIM_GetCounter(TIM2) - time_var2;
		printf("Time:      %lu us\n\r", time_var3);
		printf("C = ");
		for (i=0; i<8;i++)
			printf("%02x", c[i]);
		printf("\n\n\r");
*/
/*		printf("two byte 4x1 version\n");
		time_var2 = TIM_GetCounter(TIM2);
		LED_enc_1col(c1col, k1col, kbits);
		time_var3 = TIM_GetCounter(TIM2) - time_var2;
		printf("Time:      %lu us\n\r", time_var3);
		printf("C = ");
		for (i=0; i<8;i++)
			printf("%02x", c[i]);
		printf("\n\n\r");
*/	
 		printf("Iteration: %i\n\r", iteration);
		printf("K = ");
		for (i=0; i<kbits/8;i++)
			printf("%02x", k[i]);
		printf("\n\r");

		printf("P = ");
		for (i=0; i<8;i++)
			printf("%02x", p[i]);
		printf("\n\r");

		printf("C = ");
		for (i=0; i<8;i++)
			printf("%02x", c[i]);
		printf("\n\n\r");

		/*printf("K = ");
		for (i=0; i<kbits/8;i++)
			printf("%02x", k[i]);
		printf("\n\r");

		printf("P = ");
		for (i=0; i<8;i++)
			printf("%02x", p[i]);
		printf("\n\r");

		printf("C = ");
		for (i=0; i<8;i++)
			printf("%02x", c[i]);
		printf("\n\n\r");
*/

		//decryption
		//for (i=0; i<8; i++)
		//{
		//	k2[i] = c[i]^p[i];
		//	printf("%02x", k2[i] ^ c[i]);
		//}


		iteration++;
	}
}

void init() {
	GPIO_InitTypeDef  GPIO_InitStructure;

	// ---------- SysTick timer -------- //
	if (SysTick_Config(SystemCoreClock / 1000)) {
		// Capture error
		while (1){};
	}


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 
    TIM_TimeBaseInitTypeDef timerInitStructure; 
    timerInitStructure.TIM_Period = 84000000-1;
    timerInitStructure.TIM_Prescaler = 84; //40000;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_Cmd(TIM2, ENABLE);

	// ---------- GPIO -------- //
	// GPIOD Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	// Configure PD12, PD13, PD14 and PD15 in output pushpull mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// ------------- USB -------------- //
	USBD_Init(&USB_OTG_dev,
	            USB_OTG_FS_CORE_ID,
	            &USR_desc,
	            &USBD_CDC_cb,
	            &USR_cb);
}

/*
 * Called from systick handler
 */
void timing_handler() {
	if (time_var1) {
		time_var1--;
	}

	time_var2++;
}

/*
 * Delay a number of systick cycles (1ms)
 */
void Delay(volatile uint32_t nCount) {
	time_var1 = nCount;
	while(time_var1){};
}

/*
 * Dummy function to avoid compiler error
 */
void _init() {

}

