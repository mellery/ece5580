/* 
 *	Copyright (c) 2014. All rights are reserved by Jian Guo
 *
 *  Refer to http://eprint.iacr.org/2012/600.pdf for the design details.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL WE
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, OR CONSEQUENTIAL 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OF THE 
 * SOFTWARE.
 * 
 * Thanks to Yuichi Niwa at Nagoya University for an independent implementation, 
 * and for pointing out an error in Key Schedule.
 * 
 * This code is publicly available from: http://led.crypto.sg
 *
 * Contact: Jian Guo, ntu.guo@gmail.com
 * Last Modified: 27 June 2014
 */


#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include "led-bytes.h"
#include "LED_tables.h"

#define ROTL16(in, l) ((in) << l) ^ ((in) >> (16-l))
#define ROTR16(in, l) ((in) >> l) ^ ((in) << (16-l))
#define MASK4  0x0f
#define MASK8  0xff
#define MASK16 0xffff

#define LEDROUND(state) do {\
        unsigned long long stateIn;\
        stateIn = state;\
        state  = T0_LED[stateIn & MASK8];\
        state ^= T1_LED[(stateIn >> 8) & MASK8];\
        state ^= T2_LED[(stateIn >> 16) & MASK8];\
        state ^= T3_LED[(stateIn >> 24) & MASK8];\
        state ^= T4_LED[(stateIn >> 32) & MASK8];\
        state ^= T5_LED[(stateIn >> 40) & MASK8];\
        state ^= T6_LED[(stateIn >> 48) & MASK8];\
        state ^= T7_LED[(stateIn >> 56) & MASK8];\
} while(0);


 
void LED128table_core(const uint8_t * plaintext, const uint8_t* roundKeys128, uint8_t* ciphertext)
{
	uint64_t * state, * roundKeys;

	/* cast variables */
	*((uint64_t*)ciphertext) = *((uint64_t*)plaintext);
	state     = (uint64_t *)ciphertext;
	roundKeys = (uint64_t *)roundKeys128;

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 1 */
	state[0] ^= Tcon128LED[0];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[1];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[2];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[3];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[1];

	/* step 2 */
	state[0] ^= Tcon128LED[4];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[5];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[6];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[7];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 3 */
	state[0] ^= Tcon128LED[8];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[9];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[10];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[11];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[1];

	/* step 4 */
	state[0] ^= Tcon128LED[12];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[13];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[14];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[15];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 5 */
	state[0] ^= Tcon128LED[16];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[17];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[18];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[19];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[1];

	/* step 6 */
	state[0] ^= Tcon128LED[20];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[21];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[22];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[23];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 7 */
	state[0] ^= Tcon128LED[24];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[25];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[26];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[27];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[1];

	/* step 8 */
	state[0] ^= Tcon128LED[28];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[29];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[30];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[31];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 9 */
	state[0] ^= Tcon128LED[32];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[33];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[34];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[35];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[1];

	/* step 10 */
	state[0] ^= Tcon128LED[36];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[37];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[38];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[39];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 11 */
	state[0] ^= Tcon128LED[40];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[41];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[42];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[43];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[1];

	/* step 12 */
	state[0] ^= Tcon128LED[44];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[45];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[46];
	LEDROUND(state[0]);
	state[0] ^= Tcon128LED[47];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	return;
}



//#include "diag/Trace.h"
//#include "stm32f4xx.h"
//#include "stm32f4xx_hal.h"


int LED = 64; // to use, change this to the key size wanted, assumed to be multiple of 4.
//#define RN  32
//int RN = 32;

#if LED<=64
	#define RN 	32
#elif LED<=128
	#define RN 	48
#endif

#ifndef TS
	#define TS	8
	#define NIB	2
#endif

#define INDEX_FILTER ((1<<(NIB<<2))-1)

#if LED<64
	#define ROLTKEY(k) ((k>>(LED))||((k)<<(64-(LED))))
#elif LED==64
	#define ROLTKEY(k) (k)
#endif

const unsigned char MixColMatrix[4][4] = {
	{4,  1, 2, 2},
	{8,  6, 5, 6},
	{11,14,10, 9},
	{2,  2,15,11},
};
const unsigned char sbox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};
//was byte
const char RC[48] = {
	0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
	0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
	0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
	0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
	0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04
};
const unsigned char WORDFILTER = 0xF;

//int DEBUG = 0;
unsigned char FieldMult(unsigned char a, unsigned char b)
{
	const unsigned char ReductionPoly = 0x3;
	unsigned char x = a, ret = 0;
	int i;
	for(i = 0; i < 4; i++) {
		if((b>>i)&1) ret ^= x;
		if(x&0x8) {
			x <<= 1;
			x ^= ReductionPoly;
		}
		else x <<= 1;
	}
	return ret&WORDFILTER;
}

void AddKey(unsigned char state[4][4], unsigned char* keyBytes, int step)
{
	int i, j;
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            state[i][j] ^= keyBytes[(4*i+j+step*16)%(LED/4)];
}

void AddConstants(unsigned char state[4][4], int r)
{
	const unsigned char RC[48] = {
		0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
		0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
		0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
		0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
		0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04
	};
	state[1][0] ^= 1;
	state[2][0] ^= 2;
	state[3][0] ^= 3;

	state[0][0] ^= (LED>>4)&0xf;
	state[1][0] ^= (LED>>4)&0xf;
	state[2][0] ^= LED & 0xf;
	state[3][0] ^= LED & 0xf;

	unsigned char tmp = (RC[r] >> 3) & 7;
	state[0][1] ^= tmp;
	state[2][1] ^= tmp;
	tmp =  RC[r] & 7;
	state[1][1] ^= tmp;
	state[3][1] ^= tmp;
}

void SubCell(unsigned char state[4][4])
{
	int i,j;
	for(i = 0; i < 4; i++)
		for(j = 0; j <  4; j++)
			state[i][j] = sbox[state[i][j]];
}

void ShiftRow(unsigned char state[4][4])
{
	int i, j;
	unsigned char tmp[4];
	for(i = 1; i < 4; i++) {
		for(j = 0; j < 4; j++)
			tmp[j] = state[i][j];
		for(j = 0; j < 4; j++)
			state[i][j] = tmp[(j+i)%4];
	}
}

uint64_t Ssbox[TS][1<<(NIB<<2)] = {{ 0 }};
uint64_t RRC[RN] = { 0 };
void BuildTableSCShRMCS()
{
	int r;
	uint64_t x;
	for(r = 0; r < TS; r++){ //  build the table for Ssbox[r][.]
		for(x = 0; x != (1<<(NIB<<2)); x++) // for all values of the row
		{
			uint8_t c[NIB];
			uint64_t result = 0;
			int i, j;
			for(i = 0; i < NIB; i++){ // for all cells of this row value, we are dealing with the cell[r][i] after shiftrow
				if(r* NIB + i >= 16) break;
				c[i] = sbox[(x>>(i*4))&0xF];
				int row = (r*NIB + i) >> 2;
				int col = (((r*NIB + i) & 3) + 4 - row)&3; // column after shift row
				for(j = 0; j < 4; j++) {
					uint8_t v = FieldMult(c[i], MixColMatrix[j][row]);
					result ^= ((((uint64_t) v)&0xFULL) << (j*16+col*4));
				}
			}
			Ssbox[r][x] = result;
		}
	}

	// precompute the constants
	for(r = 0; r < RN; r++)
	{
		uint64_t t = 0;
		t |= (uint64_t) (LED>>4)&0xF;
		t |= ((uint64_t) 1^((LED>>4) & 0xFF)) << 16;
		t |= ((uint64_t) 2^(LED&0xF)) << 32;
		t |= ((uint64_t) 3^(LED&0xF)) << 48;

		t |= ((uint64_t) (RC[r] >> 3) & 7) << 4;
		t |= ((uint64_t) (RC[r] >> 3) & 7) << (4+32);
		t |= ((uint64_t) (RC[r] >> 0) & 7) << (4+16);
		t |= ((uint64_t) (RC[r] >> 0) & 7) << (4+16+32);
		RRC[r] = t;
	}

	// build the round keys
	// for(r = 0; r < RN; r++) { }
}

inline void SCShRMCS(uint64_t* s)
{
#if (NIB==2) && defined(_MEM_)
	*s = Ssbox[0][((uint8_t *)s)[0]] ^ Ssbox[1][((uint8_t *)s)[1]] ^ Ssbox[2][((uint8_t *)s)[2]] ^ Ssbox[3][((uint8_t *)s)[3]] ^ Ssbox[4][((uint8_t *)s)[4]] ^ Ssbox[5][((uint8_t *)s)[5]] ^ Ssbox[6][((uint8_t *)s)[6]] ^ Ssbox[7][((uint8_t *)s)[7]];
	//uint64_t x  = Ssbox[0][((uint8_t *)s)[0]]; x ^= Ssbox[1][((uint8_t *)s)[1]]; x ^= Ssbox[2][((uint8_t *)s)[2]]; x ^= Ssbox[3][((uint8_t *)s)[3]]; x ^= Ssbox[4][((uint8_t *)s)[4]]; x ^= Ssbox[5][((uint8_t *)s)[5]]; x ^= Ssbox[6][((uint8_t *)s)[6]]; x ^= Ssbox[7][((uint8_t *)s)[7]]; *s = x;
	//*s = Ssbox[0][((uint8_t *)s)[7]] ^ Ssbox[1][((uint8_t *)s)[6]] ^ Ssbox[2][((uint8_t *)s)[5]] ^ Ssbox[3][((uint8_t *)s)[4]] ^ Ssbox[4][((uint8_t *)s)[3]] ^ Ssbox[5][((uint8_t *)s)[2]] ^ Ssbox[6][((uint8_t *)s)[1]] ^ Ssbox[7][((uint8_t *)s)[0]];
#elif (NIB==4) && defined(_MEM_)
	*s = Ssbox[0][((uint16_t *)s)[0]] ^ Ssbox[1][((uint16_t *)s)[1]] ^ Ssbox[2][((uint16_t *)s)[2]] ^ Ssbox[3][((uint16_t *)s)[3]];
#else
	uint64_t x = 0, tmp = *s;
	int i;
	for(i = 0; i < TS; i++){
		x ^= Ssbox[i][tmp&INDEX_FILTER];
		tmp >>= (NIB<<2);
	}
	*s = x;
#endif
}

#if LED<=64
uint64_t LEDRound(uint64_t state, uint64_t key)
#elif LED<=128
uint64_t LEDRound(uint64_t state, uint64_t key[2])
#endif
{
#ifdef _PRINT_
	printf("--------- input state --------------\n");
	printState(state);
	printf("-- input key with key size = %02d ---\n", LED);
#endif
	int i, j;
#if LED<=64
	register uint64_t lkey = key & ((((uint64_t) 1) << LED)-1);
#ifdef _PRINT_
	printState(lkey);
#endif
	for(i = LED; i < 64; i+=i)
		lkey = (lkey<<i)^lkey;
#if NOKEY
#else
	state ^= lkey;
#endif
#elif LED<=128
	uint64_t lkeyl = key[0];
	uint64_t lkeyr, tkey;
	lkeyr = key[1] & ((((uint64_t) 1)<<(LED&0x3F))-1);
#ifdef _PRINT_
	printState(lkeyl);
	printState(lkeyr);
#endif
	lkeyr |= lkeyl<<(LED-64);
#if NOKEY
#else
	//state ^= key[0];
	state ^= lkeyl;
	tkey  = lkeyl;
	lkeyl = lkeyr|(tkey<<(LED-64));
	lkeyr = tkey>>(128-LED);
#endif
	uint8_t kindex = 1;
#endif
#ifdef _PRINT_
		printf("--------------------------------------\n");
#endif
	for(i = 0; i < RN; i+=4)
	{
		state ^= RRC[i+0]; 
#ifdef _PRINT_
		printf("----round %02d after adding constant----\n", i);
		printState(state);
#endif
		SCShRMCS(&state);
#ifdef _PRINT_
		printf("----round %02d output-------------------\n", i);
		printState(state);
#endif
		state ^= RRC[i+1]; 
#ifdef _PRINT_
		printf("----round %02d after adding constant----\n", i+1);
		printState(state);
#endif
		SCShRMCS(&state);
#ifdef _PRINT_
		printf("----round %02d output--------------------\n", i+1);
		printState(state);
#endif
		state ^= RRC[i+2]; 
#ifdef _PRINT_
		printf("----round %02d after adding constant----\n", i+2);
		printState(state);
#endif
		SCShRMCS(&state);
#ifdef _PRINT_
		printf("----round %02d output-------------------\n", i+2);
		printState(state);
#endif
		state ^= RRC[i+3]; 
#ifdef _PRINT_
		printf("----round %02d after adding constant----\n", i+3);
		printState(state);
#endif
		SCShRMCS(&state);
#ifdef _PRINT_
		printf("----round %02d output-------------------\n", i+3);
		printState(state);
#endif
#if LED<=64
#if NOKEY
#else
		lkey = ROLTKEY(lkey);
		state ^= lkey;
#endif
#elif LED<=128
#if NOKEY
#else
		//state ^= key[kindex]; kindex ^= 0x01;
		state ^= lkeyl;
		tkey  = lkeyl;
		lkeyl = lkeyr|(tkey<<(LED-64));
		lkeyr = tkey>>(128-LED);
#endif
#endif
	}
#ifdef _PRINT_
	printf("------------- final output -----------\n");
	printState(state);
	printf("------------- process ends -----------\n");
#endif
	return state;
}

void MixColumn(unsigned char state[4][4])
{
	int i, j, k;
	unsigned char tmp[4];
	for(j = 0; j < 4; j++){
		for(i = 0; i < 4; i++) {
			unsigned char sum = 0;
			for(k = 0; k < 4; k++)
				sum ^= FieldMult(MixColMatrix[i][k], state[k][j]);
			tmp[i] = sum;
		}
		for(i = 0; i < 4; i++)
			state[i][j] = tmp[i];
	}
}

void LED80_enc(unsigned char* input, const unsigned char* userkey)
{
	LED_enc(input, userkey, 80);
}

void LED_enc(unsigned char* input, const unsigned char* userkey, int ksbits)
{
	unsigned char state[4][4];
	unsigned char keyNibbles[32];
	int i;
	for(i = 0; i < 16; i++) {
		if(i%2) state[i/4][i%4] = input[i>>1]&0xF;
		else state[i/4][i%4] = (input[i>>1]>>4)&0xF;
	}

	//memset(keyNibbles, 0, 32);
	for(i = 0; i < ksbits/4; i++){
		if(i%2) keyNibbles[i] = userkey[i>>1]&0xF;
		else keyNibbles[i] = (userkey[i>>1]>>4)&0xF;
	}
	LED = ksbits;
	//int RN = 48;
	//if(LED <= 64)
	//	RN = 32;
	int j;
	AddKey(state, keyNibbles, 0);
	for(i = 0; i < RN/4; i++){
		for(j = 0; j < 4; j++)
		{
			AddConstants(state, i*4+j);
			SubCell(state);
			ShiftRow(state);
			MixColumn(state);
		}
		AddKey(state, keyNibbles, i+1);
	}
	for(i = 0; i < 8; i++)
		input[i] = ((state[(2*i)/4][(2*i)%4] & 0xF) << 4) | (state[(2*i+1)/4][(2*i+1)%4] & 0xF);
}

void TestVectors(int kbits)
{
	unsigned char p[8];
	unsigned char c[8];
	unsigned char k[16];
	int n;
	/*for(n = 1; n < 10; n++)
	{
		int i;
		for(i = 0; i < 8; i++) c[i] = p[i] = rand() & 0xff;
		for(i = 0; i < 16; i++)k[i] = rand() & 0xff;
		//trace_printf ("test===================================\n");
		//trace_printf("K= ");
		//for(i = 0; i < kbits/8; i++) trace_printf("%02x", k[i]); trace_printf("\n");
		//trace_printf("P = "); for(i = 0; i < 8; i++) trace_printf("%02x", p[i]); trace_printf("\n");
		LED_enc(c, k, kbits);
		//trace_printf("C = "); for(i = 0; i < 8; i++) trace_printf("%02x", c[i]); trace_printf("\n\n");

	}*/
	int i;


	for(i = 0; i < 8; i++) c[i] = p[i] = 0x00; //& 0xff;
			for(i = 0; i < 16; i++)k[i] = 0x00; //rand() & 0xff;
			//trace_printf ("test===================================\n");
			//trace_printf("K= ");
			//for(i = 0; i < kbits/8; i++) trace_printf("%02x", k[i]); trace_printf("\n");
			//trace_printf("P = "); for(i = 0; i < 8; i++) trace_printf("%02x", p[i]); trace_printf("\n");
			LED_enc(c, k, kbits);
			//trace_printf("C = "); for(i = 0; i < 8; i++) trace_printf("%02x", c[i]); trace_printf("\n\n");


}

/*int led_main(int argc, char*argv[])
{
	//printf("LED-64: \n");
	TestVectors(64);
	//printf("\n\nLED-80: \n");
	TestVectors(80);
	//printf("\n\nLED-128: \n");
	TestVectors(128);
	return 0;
}
*/
