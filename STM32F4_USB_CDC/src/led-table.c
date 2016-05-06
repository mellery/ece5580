#include <stdlib.h>
#include <stdint.h>
#include "led-table.h"
#include "LED_tables.h"

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

void LED64table_core(const uint8_t* plaintext, const uint8_t* roundKeys64, uint8_t* ciphertext)
{
	uint64_t * state, * roundKeys;

	/* cast variables */
	*((uint64_t*)ciphertext) = *((uint64_t*)plaintext);
	state     = (uint64_t *)ciphertext;
	roundKeys = (uint64_t *)roundKeys64;

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 1 */
	state[0] ^= Tcon64LED[0];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[1];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[2];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[3];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 2 */
	state[0] ^= Tcon64LED[4];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[5];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[6];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[7];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 3 */
	state[0] ^= Tcon64LED[8];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[9];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[10];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[11];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 4 */
	state[0] ^= Tcon64LED[12];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[13];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[14];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[15];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 5 */
	state[0] ^= Tcon64LED[16];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[17];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[18];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[19];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 6 */
	state[0] ^= Tcon64LED[20];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[21];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[22];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[23];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 7 */
	state[0] ^= Tcon64LED[24];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[25];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[26];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[27];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	/* step 8 */
	state[0] ^= Tcon64LED[28];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[29];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[30];
	LEDROUND(state[0]);
	state[0] ^= Tcon64LED[31];
	LEDROUND(state[0]);

	/* addRoundKey */
	state[0] ^= roundKeys[0];

	return;
}

 
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
