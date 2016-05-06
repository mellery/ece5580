
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
void AddKeyBytes_1col(unsigned short state[4], unsigned short* keyBytes, int step)
{
    int i, j;
    for(i = 0; i < 4; i++)
        state[i] ^= keyBytes[(i+step*16)%(LED/16)];
}


void AddConstantsBytes_1col(unsigned short state[4], int r)
{
/*	const unsigned char RC[48] = {
		0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
		0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
		0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
		0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
		0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04
	};
*/
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

void SubCellBytes_1col(unsigned short state[4])
{
    int i;
    for(i = 0; i < 4; i++)
        state[i] = (((sbox[(state[i]>>12)&0xF])<<12) | ((sbox[((state[i]>>8)&0xF)])<<8) | ((sbox[((state[i]>>4)&0xF)])<<4) | (sbox[(state[i]&0xF)]));
        //state[i][j] = ( ((sbox[(state[i][j]>>12)&0xF])<<12) | ((sbox[((state[i][j]>>8)&0xF)])<<8) | ((sbox[((state[i][j]>>4)&0xF)])<<4) | (sbox[(state[i][j]&0xF)]) );
}


void ShiftRowBytes_1col(unsigned short state[4])
{
	state[1] = (((state[1] << 4) | (state[1] >> 12)) & 0xFFFF);
	state[2] = (((state[2] << 8) | (state[2] >> 8)) & 0xFFFF);
	state[3] = (((state[3] << 12) | (state[3] >> 4)) & 0xFFFF);

	//state[0][1] = (((state[0][1] << 4) | (state[0][1] >> 12)) & 0xFFFF);
	//state[1][0] = (((state[1][0] << 8) | (state[1][0] >> 8)) & 0xFFFF);
	//state[1][1] = (((state[1][1] << 12) | (state[1][1] >> 4)) & 0xFFFF);
}

void MixColumnBytes_1col(unsigned short state[4]) 
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

void LED_enc_1col(unsigned short* input, unsigned short* userkey, int ksbits)
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
}

