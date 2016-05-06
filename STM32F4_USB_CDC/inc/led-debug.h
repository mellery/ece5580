
void print_state_1col(unsigned short state[4])
{
    int i;
    for (i=0;i<4;i++)
    {
	printf("%i |\t", state[i]);
	for (int j=3; j>=0; j--)
            printf(" %i", state[i]>>(4*j)&0xf);
//	printf(" | %i %i %i %i", HI_NIBBLE(state[i][0]), LO_NIBBLE(state[i][0]), HI_NIBBLE(state[i][1]), LO_NIBBLE(state[i][1]));
        printf("\n");
    }
}

void print_state16(unsigned short state[4][2])
{
    #define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
    #define LO_NIBBLE(b) ((b) & 0x0F)

    int i;
    for (i=0;i<4;i++)
    {
        for (int x=0;x<2;x++)
		printf("%i ", state[i][x]);
	printf(" | %i %i %i %i", HI_NIBBLE(state[i][0]), LO_NIBBLE(state[i][0]), HI_NIBBLE(state[i][1]), LO_NIBBLE(state[i][1]));
        printf("\n");
    }
}

void print_state(unsigned char state[4][4])
{
    int i;
    for (i=0;i<4;i++)
    {
        for (int x=0;x<4;x++)
		printf("%i ", state[i][x]);
        printf("\n");
    }
}
