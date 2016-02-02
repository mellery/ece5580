/* Thanks for Valentina Banciu for pointing out 
   some bugs in previous versions
	Last updated: 17 Jan 2014
*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "LED.h"


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


uint64_t RoundKeys[RN] = { 0 };
const byte MixColMatrix[4][4] = {
	{4,  1, 2, 2},
	{8,  6, 5, 6},
	{11,14,10, 9},
	{2,  2,15,11},
};
const byte sbox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};
const byte RC[48] = {
	0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
	0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
	0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
	0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
	0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04
};
const byte WORDFILTER = 0xF;

int DEBUG = 0;
inline byte FieldMult(byte a, byte b)
{
	const byte ReductionPoly = 0x3;
	byte x = a, ret = 0;
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

void SubCell(byte state[4][4])
{
	int i,j;
	for(i = 0; i < 4; i++)
		for(j = 0; j <  4; j++)
			state[i][j] = sbox[state[i][j]];
}

void ShiftRow(byte state[4][4])
{
	int i, j;
	byte tmp[4];
	for(i = 1; i < 4; i++) {
		for(j = 0; j < 4; j++)
			tmp[j] = state[i][j];
		for(j = 0; j < 4; j++)
			state[i][j] = tmp[(j+i)%4];
	}
}

void MixColumn(byte state[4][4])
{
	int i, j, k;
	byte tmp[4];
	for(j = 0; j < 4; j++){
		for(i = 0; i < 4; i++) {
			byte sum = 0;
			for(k = 0; k < 4; k++)
				sum ^= FieldMult(MixColMatrix[i][k], state[k][j]);
			tmp[i] = sum;
		}
		for(i = 0; i < 4; i++)
			state[i][j] = tmp[i];
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

void printState(uint64_t s)
{
	int i;
	for(i = 0; i < 16; i++)
	{
		printf("%X ", (byte)s&0xF);
		s>>=4;
		if((i&3) == 3) printf("\n");
	}
	printf("\n");
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
	uint64_t lkey = key & ((((uint64_t) 1) << LED)-1);
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

#define SAMPLE	(1<<18)
void TestSpeed()
{
	uint64_t mess[SAMPLE];
	double bytes = SAMPLE*8;
	int i;
	for(i = 0; i < (SAMPLE); i++)
		mess[i] = SHA256RandomLong();
	uint64_t state = 0;
	tstart();
#if LED<=64
	for(i = 0; i < (SAMPLE); i++)
		state = LEDRound(state, mess[i]);
#elif LED<=128
	for(i = 0; i < (SAMPLE>>1); i++)
		state = LEDRound(state, mess+(i<<1));
	bytes /=2;
#endif
	double tt = (double) tdiff();
	double speed = tt / bytes;
	printf("%.5f cycles per byte.\n", speed);
}

void TestVectors()
{
#if LED<=64
	uint64_t state = 0, keys = 0;
	LEDRound(state, keys);
	printf("\n\n");
	state = 0xFEDCBA9876543210ULL;
	keys  = 0xFEDCBA9876543210ULL;
	LEDRound(state, keys);
#elif LED<=128
	uint64_t state = 0, keys[2] = { 0, 0};
	LEDRound(state, keys);
	printf("\n\n");
	state = 0xFEDCBA9876543210ULL;
	keys[0] = keys[1] = 0xFEDCBA9876543210ULL;
	LEDRound(state, keys);
#endif
}

void printSSBOX()
{
	int i, j;
	printf("const uint64_t Ssbox[8][1<<8] = {\n");
	for(i = 0; i < TS; i++)
	{
		printf("{ ");
		for(j = 0; j < (1<<(NIB<<2)); j++){
			printf("0x%016llXLL", (unsigned long long) Ssbox[i][j]);
			if(j+1 != (1<<(NIB<<2))) printf(", ");
		}
		printf("}");
		if(i+1 != TS) printf(",");
		printf("\n");
	}
	printf("};\n");
	printf("const uint64_t RRC[48] = {\n");
	for(i = 0; i < 48; i++){
		printf("0x%016llXLL", (unsigned long long) RRC[i]);
		if(i+1 !=48) printf(", ");
	}
	printf("};\n");
}

int main(int argc, char*argv[])
{
	srand(time(0));
	BuildTableSCShRMCS();
#ifdef _PRINT_
	TestVectors();
#else
	//printSSBOX();
	TestSpeed();
#endif
	return 0;
}
