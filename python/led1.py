__author__ = 'praneethgurram'
import random

from ctypes import c_longlong as ll

MixColMatrix = ((4, 1, 2, 2), (8,6,5,6), (11, 14, 10, 9), (2, 2, 15, 11))
LED = 64
sbox = (12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2)

TS = 8
NIB = 2

INDEX_FILTER = ((1<<(NIB<<2))-1)

#TODO refactor RN
if LED <= 64:
    RN = 32
else:
    RN = 48

RC = [	0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
	0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
	0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
	0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
	0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04]

RRC = [0] * RN

#Ssbox = [[0 for x in range(TS)] for x in range(1<<(NIB<<2))]
Ssbox = [[0 for x in range(1<<(NIB<<2))] for x in range(TS)]


def BuildTableSCShRMCS():
    global Ssbox

    print "Building Table SCShRMCS"
 
    for r in range (0, TS):
        x = 0
        while x != (1<<(NIB<<2)):
            c = [0]*NIB
            result = 0
            for i in range(0,NIB):
                if (r*NIB+1 >= 16):
                    break
                c[i] = sbox[(x>>(i*4))&0xF]
                row = (r*NIB + i) >> 2
                col = (((r*NIB + i) & 3) + 4 - row) &3
                for j in range(0,4):
                    v = FieldMult(c[i], MixColMatrix[j][row])
                    result ^= ((v&0xF) << (j*16+col*4))	
            Ssbox[r][x] = result
            x = x+1


    #precompute the constants
    for r in range(0,RN):
        t = 0
       
        t |= (LED>>4)&0xF;	#type uint64_t
        t |= (1^((LED>>4) & 0xFF)) << 16;
        t |= (2^(LED&0xF)) << 32;
        t |= (3^(LED&0xF)) << 48;

        t |= ((RC[r] >> 3) & 7) << 4;
        t |= ((RC[r] >> 3) & 7) << (4+32);
        t |= ((RC[r] >> 0) & 7) << (4+16);
        t |= ((RC[r] >> 0) & 7) << (4+16+32);
        RRC[r] = t;

def SCShRMCS(s):
    global NIB
 
    sbytes = list((s >> i) & 0xFF for i in range(0,64,8))

    if (NIB==2): #&& defined(_MEM_)
        s = Ssbox[0][sbytes[0]] ^ Ssbox[1][sbytes[1]] ^ Ssbox[2][sbytes[2]] ^ Ssbox[3][sbytes[3]] ^ Ssbox[4][sbytes[4]] ^ Ssbox[5][sbytes[5]] ^ Ssbox[6][sbytes[6]] ^ Ssbox[7][sbytes[7]]

    elif (NIB==4): #&& defined(_MEM_)
        s = Ssbox[0][(s)[0]] ^ Ssbox[1][(s)[1]] ^ Ssbox[2][(s)[2]] ^ Ssbox[3][(s)[3]]

    else:
	x = 0
        tmp = s 
	for i in range(0,TS):
            x ^= Ssbox[i][tmp&INDEX_FILTER]
            tmp >>= (NIB<<2)
	
	s = x

    return s


#TODO: make work for LED>64
def LEDRound(state, key):
    lkey = key & ((1 << LED)-1)
    for i in range(LED,64,LED): #TODO increment by i+=i?
        print "in loop that doesn't work"
        lkey = (lkey<<i)^lkey

    #def NOKEY?
    state ^= lkey

    for i in range(0,RN,4):
        state ^= RRC[i+0]
        state = SCShRMCS(state)

        state ^= RRC[i+1]        
        state = SCShRMCS(state)

        state ^= RRC[i+2]        
        state = SCShRMCS(state)

        state ^= RRC[i+3]        
        state = SCShRMCS(state)

        #lkey = ROLTKEY(lkey)
        if LED<64:
            lkey = ((lkey>>(LED))|((lkey)<<(64-(LED))))


        state ^= lkey

    #print "STATE",state
    #print hex(state)
   
    #print "calling printState"
    printState(state)
    
    return state

def printState(s):
    sbytes = list((s >> i) & 0xFF for i in range(0,64,8))

    s_str = ""
    for byte in sbytes:
        temp = hex(byte).split('x')[1]
        temp = temp.replace('L','')
        if len(temp) == 1: #add zero padding
            temp = '0'+temp
        rev = temp[::-1]
        s_str = s_str + rev #temp #c_str + hex(byte).split('x')[1]

    s_str = s_str.replace('L', '') # remove Ls



    print 'S=',s_str
 

def printSSBOX():
    print "Ssbox\n"
    print Ssbox
    for s in range(0,len(Ssbox)):
        for s2 in range(0,len(Ssbox[s])):
            print hex(Ssbox[s][s2])
    print "\n\nRRC\n\n"
    print RRC

    #for s in range(0,1):#len(Ssbox)):
    #    for s2 in range(0,len(Ssbox[s])):
    #        print hex(Ssbox[s][s2])

    #print "---"
    #for r in range(0,len(RRC)):
    #    print hex(RRC[r])

    #print "const uint64_t Ssbox[8][1<<8] = {\n"
    #for i in range(0,TS):
    #    print "{ "
    #    for j in range(0,(1<<(NIB<<2))):
    #        print Ssbox[i][j]
    #        if (j+1 != (1<<(NIB<<2))):
    #            print ", "
    #    print "}"
    #    if i+1 != TS:
    #        print ","
    #    print "\n"
    #print "};\n\n\n"

    #print "const uint64_t RRC[48] = {\n"
    #for i in range(0,RN):
    #    print "0x", RRC[i]
    #    if i+1 !=RN:
    #        print ","
    #print "};\n"

def FieldMult(a, b):
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
    global LED
    for i in  range(0,4):
        for j in range(0,4):
            #state[i][j]^=keybytes[i][j]
            state[i][j]^=keybytes[(4*i+j+step*16)%(LED/4)]

def AddConstants(state, r):
        global LED
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
	for j in range(0,4):
		for i in range(0,4):
			mySum = 0
			for k in range(0,4):
				mySum ^= FieldMult(MixColMatrix[i][k], state[k][j])
			tmp[i] = mySum
		for i in range(4):
			state[i][j] = tmp[i]

def LED_enc(myinput, userkey, ksbits):

	state = [[0 for x in range(0,4)] for x in range(0,4)]
        #keynibbles2d = [[0 for x in range(0,4)] for x in range(0,4)]
        keynibbles = [0]*32

	for i in range(0,16):
            if i%2:
                state[i/4][i%4] = myinput[i>>1]&0xF
            else:
                state[i/4][i%4] = myinput[i>>1]>>4&0xF

        #print "ksbits/4:",ksbits/4
	for i in range(0,ksbits/4):
            if i%2:
#                keynibbles2d[i/4][i%4] = userkey[i>>1]&0xF
                keynibbles[i] = userkey[i>>1]&0xF
            else:
#                keynibbles2d[i/4][i%4] = (userkey[i>>1]>>4)&0xF
                keynibbles[i] = (userkey[i>>1]>>4)&0xF


#        print keynibbles2d
#        for row in range(0,len(keynibbles2d)):
            #print keynibbles2d[row]
#            for col in range(0,len(keynibbles2d[row])):
#                keynibbles.append(keynibbles2d[row][col])
        #print len(keynibbles2d)    
        #print "==="
        while len(keynibbles) < 32:
            keynibbles.append(0)
#        print keynibbles


	#TODO calc RN
        RN = 48
        if ksbits <= 64:
    	    RN = 32

	AddKey(state, keynibbles, 0)

	for i in range(0,RN/4):
            for j in range(0,4):
		AddConstants(state, i*4+j)
		SubCell(state)
		ShiftRow(state)
		MixColumn(state)

	    AddKey(state, keynibbles, i+1)

	for i in range(0,8):
	    myinput[i] = ((state[(2*i)/4][(2*i)%4] & 0xF) << 4) | (state[(2*i+1)/4][(2*i+1)%4] & 0xF)

def TestVectors(kbits, p=None, k=None, answer=None):

    c=[0]*8

    if (p is None) and (k is None):

        p=[0]*8
        k=[0]*16

        for n in range(1,2):#0):
            for i in range(0,8):
                p[i] = (random.getrandbits(8) & 0xFF)
                c[i]=p[i]
    else:
        for i in range(0,8):
            c[i]=p[i]

    k_str = ""
    for byte in k:
        k_str = k_str + hex(byte).split('x')[1]
    print 'K=', k_str

    p_str = ""
    for byte in p:
        p_str = p_str + hex(byte).split('x')[1]
    print 'P=',p_str

    LED_enc(c,k,kbits)

    c_str = ""
    for byte in c:
        temp = hex(byte).split('x')[1]
        if len(temp) == 1: #add zero padding
            temp = '0'+temp
        c_str = c_str + temp #c_str + hex(byte).split('x')[1]
    print 'C=',c_str

    if answer != None:
        print "checking result matches expected value"
        if c_str == answer:
            print "PASS: ", c_str
        else:
            print "FAIL: ", c_str, "!=", answer, c

        print "--"

def main():

    global LED
    k_vec = []
    p_vec = []
    c_vec = [] 

    f = open('LED2-byte-TestVectors.txt')

    for line in f:
        l = line.strip("\n")

        if "K = " in l:
            k_vec.append(l.split(' = ')[1])

        if "P = " in l:
            p_vec.append(l.split(' = ')[1])

        if "C = " in l:
            c_vec.append(l.split(' = ')[1])

    for v in range(0,len(p_vec)):

        p = [p_vec[v][i:i+2] for i in range(0, len(p_vec[v]), 2)]
        for x in range(0,len(p)):
            p[x]=int(p[x],16)

        k = [k_vec[v][i:i+2] for i in range(0, len(k_vec[v]), 2)]
        for x in range(0,len(k)):
            k[x]=int(k[x],16)

        if len(k) == 8:
            print "LED-64"
            LED = 64
            kbits = 64

        elif len(k) == 10:
            print "LED-80"
            LED = 80
            kbits = 80

        elif len(k) == 16:
            print "LED-128"
            LED = 128
            kbits = 128

        TestVectors(kbits,p,k, c_vec[v])

    print "=============="
    LED = 64
    print "LED-64: \n"
    print "using random values in testvectors"
    TestVectors(64)

    kbits = 64
    p=[0]*8
    k=[0]*16

    p = [0x11,0xe1, 0x1a,0x1c,0x1f,0x23,0xf8,0x29]
    k = [0xf8,0xa4,0x1b,0x13,0xb5,0xca,0x4e,0xe8,0x98,0x32,0x38,0xe0,0x79,0x4d,0x3d,0x34]

    print "using hardcoded values in testvectors"
    TestVectors(kbits,p,k)

    p = [0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00]
    k = [0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00]
    print "p and k all zeroes in testvectors"
    TestVectors(kbits,p,k)

    #print "p and k 0-9,A-F"
    #p = [0x00,0x01, 0x02,0x03,0x04,0x05,0x06,0x07]
    #k = [0xf8,0xa4,0x1b,0x13,0xb5,0xca,0x4e,0xe8,0x98,0x32,0x38,0xe0,0x79,0x4d,0x3d,0x34]
    #TestVectors(kbits,p,k)

    #print "LED-80: \n"
    #TestVectors(80)

    #print "LED-128: \n"
    #TestVectors(128)

    print "\n\nCalling Optimized Version\n"

    BuildTableSCShRMCS()

    #print "Calling printSSBOX\n"
    #printSSBOX()

    state = 0
    keys = 0
    print "state = ", state
    print "keys = ", keys
    LEDRound(state, keys)
    
    

if __name__ == "__main__":
    main()





