__author__ = 'praneethgurram'
import random

MixColMatrix = ((4, 1, 2, 2), (8,6,5,6), (11, 14, 10, 9), (2, 2, 15, 11))
LED = 64
sbox = (12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2)

def FieldMult(a, b):
        #print "in FieldMult", a,b
	ReductionPoly = 0x3
	x = a	#abs?
	ret = 0
	for i in range(4):
		if (b>>i)&1:
			ret^=x
		if x&0x8:
			x <<= 1
			x ^= ReductionPoly
		else:
			x <<= 1
	return ret&0xF

def AddKey(state, keybytes,step):
    for i in  range(0,4):
        for j in range(0,4):
            state[i][j]^=keybytes[i][j]
	

def AddConstants(state, r):
	RC = (0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
		  0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
		  0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
		  0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
		  0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04)
	state[1][0] ^= 0x01
	state[2][0] ^= 0x02
	state[3][0] ^= 0x03

	state[0][0] ^= (LED>>4) & 0xF
	state[1][0] ^= (LED>>4) & 0xF
	state[2][0] ^= LED & 0xF
	state[3][0] ^= LED & 0xF

	tmp = (RC[r]>>3) & 0x7
	state[0][1] ^= tmp
	state[2][1] ^= tmp
	tmp =  RC[r] & 0x7
	state[1][1] ^= tmp
	state[3][1] ^= tmp

def SubCell(state):
    for i in range(0,4):
        for j in range(0,4):
            state[i][j]=sbox[state[i][j]]

def ShiftRow(state):
    tmp=[0]*4
    for i in range(0,4):
        for j in range(0,4):
            tmp[j]= state[i][j]
        for j in range(0,4):
            state[i][j]=tmp[(j+i)%4]

def MixColumn(state):
        tmp=[0]*4
        #print "in MixColumn"
	for j in range(0,4):
		for i in range(0,4):
			mySum = 0
			for k in range(0,4):
                                #print "i,j,k",i,j,k
				mySum ^= FieldMult(MixColMatrix[i][k], state[k][j])
                        #print "setting temp[i] to sum", i,mySum
			tmp[i] = mySum
		for i in range(4):
			state[i][j] = tmp[i]

def LED_enc(p_input, userkey, ksbits):
	state = [[0 for x in range(0,4)] for x in range(0,4)]
        #print state
	keynibbles = [[0 for x in range(0,4)] for x in range(0,4)]
        #print keynibbles

	for i in range(0,16):
		if i%2:
			state[i/4][i%4] = p_input[i>>1]&0xF
		else:
			state[i/4][i%4] = (p_input[i>>1]>>4)&0xF
			
	#print state 

	for i in range(0,16):
		if i%2:
			keynibbles[i/4][i%4] = userkey[i>>1]&0xF
		else:
			keynibbles[i/4][i%4] = (userkey[i>>1]>>4)&0xF
			
	#print keynibbles

	#print "post ksbits/4 loop"
	#TODO calc RN
	RN = 32
	AddKey(state, keynibbles, 0)
	print state
	#print "post AddKey"

	for i in range(0,8):
		for j in range(0,4):
			#print "i,j: ",i,j
			AddConstants(state, i*4+j)
                        #print "post AddConstants"
			SubCell(state)
                        #print "post SubCell"
			ShiftRow(state)
                        #print "post ShiftRow"
			MixColumn(state)
                        #print "post MixColumn"

		AddKey(state, keynibbles, i+1)


        #print "post nested ij for loop"

	for i in range(0,8):
		p_input[i] = ((state[(2*i)/4][(2*i)%4] & 0xF) << 4) | (state[(2*i+1)/4][(2*i+1)%4] & 0xF)

        #print "end of function"

def TestVectors(kbits):
    p=[0]*8
    c=[0]*8
    k=[0]*16
    #print p,c,k
    #for n in range(1,10):
        #print "n now",n
        #for i in range(0,8):
        #    p[i] = (random.getrandbits(8) & 0xFF)
        #    p[i]=p[i] & 0xff
        #    c[i]=p[i]
        #    print i,c[i]

    p[0]=c[0]=0x01
    p[1]=c[1]=0x23
    p[2]=c[2]=0x45
    p[3]=c[3]=0x67
    p[4]=c[4]=0x89
    p[5]=c[5]=0xab
    p[6]=c[6]=0xcd
    p[7]=c[7]=0xef
  
    k[0]=0x01
    k[1]=0x23
    k[2]=0x45
    k[3]=0x67
    k[4]=0x89
    k[5]=0xab
    k[6]=0xcd
    k[7]=0xef
    k[8]=0x00
    k[9]=0x00
    k[10]=0x00
    k[11]=0x00
    k[12]=0x00
    k[13]=0x00
    k[14]=0x00
    k[15]=0x00

 
    #print "p=",p,"k=",k
    #for i in range(0,16):
    #    k[i] = random.getrandbits(16) & 0xff
    print 'K=', k
    #for i in range(8):
        #print('%02x',k[i])
        #print('\n')
    print 'P=',p
        #for i in range(8):
            #print('%02x',p[i])
            #print('\n')
    LED_enc(c,k,kbits)
    print 'C=',c
        
    print '\n'

        #for i in range(0,8):
            #print('%02x',c[i])
            #print('\n\n')
#print('LED-64:')
#print('\n')
#TestVectors()

def main():
    print "in main"
    print "LED-64: \n"
    TestVectors(64)
    #print "LED-80: \n"
    #TestVectors(80)
    #print "LED-128: \n"
    #TestVectors(128)






if __name__ == "__main__":
    main()


