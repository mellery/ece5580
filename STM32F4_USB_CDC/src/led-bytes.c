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
#include "main.h"
#include "led-bytes.h"
#include "led-common.h"
#include "led-debug.h"

#define ROTL16(in, l) ((in) << l) ^ ((in) >> (16-l))
#define ROTR16(in, l) ((in) >> l) ^ ((in) << (16-l))

int LED = 64; // to use, change this to the key size wanted, assumed to be multiple of 4.

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

const unsigned char WORDFILTER = 0xF;
	

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

/*** Add Key Bytes ***/
//
//
/*void AddKeyBytes_1col(unsigned short state[4], unsigned short* keyBytes, int step)
{
    int i, j;
    for(i = 0; i < 4; i++)
        state[i] ^= keyBytes[(i+step*16)%(LED/16)];
}


void AddKeyBytes_1col22(unsigned short state[2][2], unsigned short* keyBytes, int step)
{
	int i, j;
    for(i = 0; i < 2; i++)
        for(j = 0; j < 2; j++)
            state[i][j] ^= keyBytes[(2*i+j+step*4)%(LED/16)];
}
void AddKey_2col(unsigned short state[4][2], unsigned short* keyBytes, int step)
{
    int i, j;
    for(i = 0; i < 4; i++)
        for(j = 0; j < 2; j++)
            state[i][j] ^= keyBytes[(2*i+j+step*16)%(LED/4)];
}*/
void AddKey(unsigned char state[4][4], unsigned char* keyBytes, int step)
{
	int i, j;
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            state[i][j] ^= keyBytes[(4*i+j+step*16)%(LED/4)];
}

//*** Add Constants ***/
//
//
/*void AddConstantsBytes_1col(unsigned short state[4], int r)
{
	state[0] ^= (((LED>>4)&0xf) << 12);
	state[1] ^= ((1 ^ ((LED>>4)&0xf)) << 12);
	state[2] ^= ((2 ^ (LED & 0xf)) << 12);
	state[3] ^= ((3 ^ (LED & 0xf)) << 12);

	unsigned char tmp = (RC[r] >> 3) & 7;

	state[0] ^= ((tmp & 0xF) << 8);
	state[2] ^= ((tmp & 0xF) << 8);

	tmp =  RC[r] & 7;

	state[1] ^= ((tmp & 0xF) << 8);
	state[3] ^= ((tmp & 0xF) << 8);
}


void AddConstantsBytes_1col22(unsigned short state[2][2], int r)
{
	state[0][0] ^= (((LED>>4)&0xf) << 12);
	state[0][1] ^= ((1 ^ ((LED>>4)&0xf)) << 12);
	state[1][0] ^= ((2 ^ (LED & 0xf)) << 12);
	state[1][1] ^= ((3 ^ (LED & 0xf)) << 12);

	unsigned char tmp = (RC[r] >> 3) & 7;

	state[0][0] ^= ((tmp & 0xF) << 8);
	state[1][0] ^= ((tmp & 0xF) << 8);

	tmp =  RC[r] & 7;

	state[0][1] ^= ((tmp & 0xF) << 8);
	state[1][1] ^= ((tmp & 0xF) << 8);
}

void AddConstants_2col(unsigned short state[4][2], int r)
{
    state[0][0] ^= (((LED>>4)&0xf) << 4);
    state[1][0] ^= ((1 ^ ((LED>>4)&0xf)) << 4);
    state[2][0] ^= ((2 ^ (LED & 0xf)) << 4);
    state[3][0] ^= ((3 ^ (LED & 0xf)) << 4);

    unsigned char tmp = (RC[r] >> 3) & 7;

    state[0][0] ^= tmp & 0xF;
    state[2][0] ^= tmp & 0xF;

    tmp =  RC[r] & 7;

    state[1][0] ^= tmp & 0xF;
    state[3][0] ^= tmp & 0xF;

}*/
void AddConstants(unsigned char state[4][4], int r)
{
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

/*** SubCell ***/
//
//
/*void SubCellBytes_1col(unsigned short state[4])
{
    int i;
    for(i = 0; i < 4; i++)
        state[i] = (((sbox[(state[i]>>12)&0xF])<<12) | ((sbox[((state[i]>>8)&0xF)])<<8) | ((sbox[((state[i]>>4)&0xF)])<<4) | (sbox[(state[i]&0xF)]));
        //state[i][j] = ( ((sbox[(state[i][j]>>12)&0xF])<<12) | ((sbox[((state[i][j]>>8)&0xF)])<<8) | ((sbox[((state[i][j]>>4)&0xF)])<<4) | (sbox[(state[i][j]&0xF)]) );
}


void SubCellBytes_1col22(unsigned short state[2][2])
{
	int i,j;
	for(i = 0; i < 2; i++)
		for(j = 0; j <  2; j++)
            state[i][j] = ( ((sbox[(state[i][j]>>12)&0xF])<<12) | ((sbox[((state[i][j]>>8)&0xF)])<<8) | ((sbox[((state[i][j]>>4)&0xF)])<<4) | (sbox[(state[i][j]&0xF)]) );
}

void SubCell_2col(unsigned short state[2][2])
{


    int row,col;
	for(row = 0; row < 4; row++)
		for(col = 0; col < 2; col++)
			state[row][col] = qsbox[((state[row][col]) >> 4)&0xF][(state[row][col]) & 0xF];


}*/
void SubCell(unsigned char state[4][4])
{
	int i,j;
	for(i = 0; i < 4; i++)
		for(j = 0; j <  4; j++)
			state[i][j] = sbox[state[i][j]];
}

/*** Shift Rows ***/
//
//
/*void ShiftRowBytes_1col(unsigned short state[4])
{
	state[1] = (((state[1] << 4) | (state[1] >> 12)) & 0xFFFF);
	state[2] = (((state[2] << 8) | (state[2] >> 8)) & 0xFFFF);
	state[3] = (((state[3] << 12) | (state[3] >> 4)) & 0xFFFF);

	//state[0][1] = (((state[0][1] << 4) | (state[0][1] >> 12)) & 0xFFFF);
	//state[1][0] = (((state[1][0] << 8) | (state[1][0] >> 8)) & 0xFFFF);
	//state[1][1] = (((state[1][1] << 12) | (state[1][1] >> 4)) & 0xFFFF);
}

void ShiftRowBytes_1col22(unsigned short state[2][2])
{
	state[0][1] = (((state[0][1] << 4) | (state[0][1] >> 12)) & 0xFFFF);
	state[1][0] = (((state[1][0] << 8) | (state[1][0] >> 8)) & 0xFFFF);
	state[1][1] = (((state[1][1] << 12) | (state[1][1] >> 4)) & 0xFFFF);
}

void ShiftRow_2col(unsigned short state[4][2])
{

    #define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
    #define LO_NIBBLE(b) ((b) & 0x0F)

    int row, j;
    unsigned short tmp[4][2];
    for(row = 1; row < 4; row++) 
    {
     	for(j = 0; j < 2; j++)
             tmp[row][j] = state[row][j];
    }

    row = 1;
    state[row][0] = (LO_NIBBLE(tmp[row][0]) << 4) | HI_NIBBLE(tmp[row][1]);
    state[row][1] = (LO_NIBBLE(tmp[row][1]) << 4) | HI_NIBBLE(tmp[row][0]);

    row = 2;
    state[row][0] = tmp[row][1]; 
    state[row][1] = tmp[row][0];

    row = 3;
    state[row][0] = (LO_NIBBLE(tmp[row][1]) << 4) | HI_NIBBLE(tmp[row][0]);
    state[row][1] = (LO_NIBBLE(tmp[row][0]) << 4) | HI_NIBBLE(tmp[row][1]);
}
*/
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

/*uint64_t Ssbox[TS][1<<(NIB<<2)] = {{ 0 }};
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
*/
/*inline void SCShRMCS(uint64_t* s)
{
#if (NIB==2) && defined(_MEM_)
	*s = Ssbox[0][((uint8_t *)s)[0]] ^ Ssbox[1][((uint8_t *)s)[1]] ^ Ssbox[2][((uint8_t *)s)[2]] ^ Ssbox[3][((uint8_t *)s)[3]] ^ Ssbox[4][((uint8_t *)s)[4]] ^ Ssbox[5][((uint8_t *)s)[5]] ^ Ssbox[6][((uint8_t *)s)[6]] ^ Ssbox[7][((uint8_t *)s)[7]];
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
*/
/*#if LED<=64
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
*/
/*** MixColumns ***/
//
//
/*void MixColumnBytes_1col(unsigned short state[4]) 
{
	int i, j, k, l;
	unsigned char tmp[4];
	unsigned short sum = 0;
	unsigned char z[4][4];
	unsigned char q = 0;
	for(j = 0; j < 4; j++)
	{    
        for(i = 0; i < 4; i++)
        {
            sum = 0;
            for(k = 0; k < 4; k++)
            {
                l = (12 - 4*j);
                q = ((state[k]) >> l) & 0xF;
                sum ^= FieldMult16[MixColMatrix[i][k]][q];
            }
            tmp[i] = sum;
        }
        for(i = 0; i < 4; i++)
		z[i][j] = tmp[i];
    }
	for(i = 0; i < 4; i++)
        	state[i] = ( (z[i%4][0] << 12) | (z[i%4][1] << 8) | (z[i%4][2] << 4) | (z[i%4][3]) );
}

void MixColumnBytes_1col22(unsigned short state[2][2])  //wrong
{
	int i, j, k, l;
	unsigned char tmp[4];
	unsigned short sum = 0;
	unsigned char z[4][4];
	unsigned char q = 0;
	for(j = 0; j < 4; j++)
    {
        for(i = 0; i < 4; i++)
        {
            sum = 0;
            for(k = 0; k < 4; k++)
            {
                l = (12 - 4*j);
                q = ((state[k/2][k%2]) >> l) & 0xF;
                sum ^= FieldMult16[MixColMatrix[i][k]][q];
            }
            tmp[i] = sum;
        }
        for(i = 0; i < 4; i++)
			z[i][j] = tmp[i];
    }
	for(i = 0; i < 4; i++)
        state[i/2][i%2] = ( (z[i%4][0] << 12) | (z[i%4][1] << 8) | (z[i%4][2] << 4) | (z[i%4][3]) );
}
void MixColumn_2col(unsigned short state[4][2])
{
    int i, j, k, q;
    unsigned char z[4][4];
    unsigned char tmp[4];
    for(j = 0; j < 4; j++)
    {
	if(j<2)
            q = 0;
        else
            q = 1;
   	for(i = 0; i < 4; i++)
	{
	    unsigned char sum = 0;
	    for(k = 0; k < 4; k++)
	    {
	        if(j % 2)
                    sum ^= FieldMult16[MixColMatrix[i][k]] [(state[k][q]) & 0xF];
                else
                    sum ^= FieldMult16[MixColMatrix[i][k]] [((state[k][q])>>4) & 0xF];
	    }
            tmp[i] = sum;
	}
	for(i = 0; i < 4; i++)
	     z[i][j] = tmp[i];
    }
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j+=2)
            state[i][j/2] = ((((z[i][j]) << 4) & 0xF0) | ((z[i][j+1]) & 0xF));
    }

}
*/
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

/*void LED_enc_1col(unsigned short* input, unsigned short* userkey, int ksbits)
{
	unsigned short state[4];
	int i, j;
	for(i = 0; i < 4; i++)
            state[i] = input[i]&0xFFFF;

	LED = ksbits;

	AddKeyBytes_1col(state, userkey, 0);
	for(i = 0; i < RN/4; i++)
	{
	    for(j = 0; j < 4; j++)
		{
			AddConstantsBytes_1col(state, i*4+j);
			SubCellBytes_1col(state);
			ShiftRowBytes_1col(state);
			MixColumnBytes_1col(state);
        }
		AddKeyBytes_1col(state, userkey, i+1);
	}
	for(i = 0; i < 4; i++)
		input[i] = state[i];
	print_state_1col(state);
}*/


/*void LED_enc_1col22(unsigned short* input, unsigned short* userkey, int ksbits)
{
	unsigned short state[2][2];
	int i, j;
	for(i = 0; i < 4; i++)
        state[i/2][i%2] = input[i]&0xFFFF;

	LED = ksbits;

	AddKeyBytes_1col22(state, userkey, 0);
	for(i = 0; i < RN/4; i++)
	{
	    for(j = 0; j < 4; j++)
		{
			AddConstantsBytes_1col22(state, i*4+j);
			SubCellBytes_1col22(state);
			ShiftRowBytes_1col22(state);
			MixColumnBytes_1col22(state);
        }
		AddKeyBytes_1col22(state, userkey, i+1);
	}
	for(i = 0; i < 4; i++)
		input[i] = state[i/2][i%2];
}*/

/*void LED_enc_2col(unsigned short* input, const unsigned short* userkey, int ksbits)
{
	unsigned short state[4][2];
	unsigned short keyBytes[8];	
	int i;

	//set initial state
	for(i = 0; i < 8; i++) {
		if(i%2) 
                    state[i/2][i%2] = input[i>>1]&0xFF;
		else 
                    state[i/2][i%2] = (input[i>>1]>>2)&0xFF;
	}

//	memset(keyBytes, 0, 32);	//memset units?
	for(i = 0; i < ksbits/8; i++){
		keyBytes[i] = (userkey[i])&0xFF;
	}

	LED = ksbits;

	int j;
	AddKey_2col(state, keyBytes, 0);

	for(i = 0; i < RN/4; i++){
		for(j = 0; j < 4; j++)
		{
			AddConstants_2col(state, i*4+j);
			SubCell_2col(state);
			ShiftRow_2col(state);
			MixColumn_2col(state);
		}
		AddKey_2col(state, keyBytes, i+1);
		//printf("end of round %i\n", i);
		//print_state_2col(state);
	}
	//printf("final state\n");
	//print_state_2col(state);
	for(i = 0; i < 8; i++)
		input[i] = ((state[(2*i)/4][(2*i)%4] & 0xF) << 4) | (state[(2*i+1)/4][(2*i+1)%4] & 0xF);
	//printf("final state\n");
	
}*/
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
	int i;

	for(i = 0; i < 8; i++) c[i] = p[i] = 0x00; //& 0xff;
			for(i = 0; i < 16; i++)k[i] = 0x00; //rand() & 0xff;
			LED_enc(c, k, kbits);

}

