

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
	
void AddKey_2col(unsigned short state[4][2], unsigned short* keyBytes, int step)
{
    int i, j;
    for(i = 0; i < 4; i++)
        for(j = 0; j < 2; j++)
            state[i][j] ^= keyBytes[(2*i+j+step*16)%(LED/4)];
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

}

void SubCell_2col(unsigned short state[2][2])
{


    int row,col;
	for(row = 0; row < 4; row++)
		for(col = 0; col < 2; col++)
			state[row][col] = qsbox[((state[row][col]) >> 4)&0xF][(state[row][col]) & 0xF];


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


void LED_enc_2col(unsigned short* input, const unsigned short* userkey, int ksbits)
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
	
}
