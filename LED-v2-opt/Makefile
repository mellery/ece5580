all:
	gcc LED_v2_opt.c sha2.c timer.c -DTS=8 -DNIB=2 -DLED=48  -o LED_v2_opt48   -O3 -D_MEM_
	gcc LED_v2_opt.c sha2.c timer.c -DTS=8 -DNIB=2 -DLED=64  -o LED_v2_opt64   -O3 -D_MEM_
	gcc LED_v2_opt.c sha2.c timer.c -DTS=8 -DNIB=2 -DLED=96  -o LED_v2_opt96   -O3 -D_MEM_
	gcc LED_v2_opt.c sha2.c timer.c -DTS=8 -DNIB=2 -DLED=128  -o LED_v2_opt128   -O3 -D_MEM_
	./LED_v2_opt64
#	rm -rf LED_v2_opt48 LED_v2_opt64 LED_v2_opt96 LED_v2_opt128
	
testvector:
	gcc -O3  LED_v2_opt.c -DLED=64 -o  tvLED64-v2 timer.c sha2.c -D_PRINT_ -D_TABLE_
	gcc -O3  LED_v2_opt.c -DLED=96 -o  tvLED96-v2 timer.c sha2.c -D_PRINT_ -D_TABLE_
	gcc -O3  LED_v2_opt.c -DLED=128 -o tvLED128-v2 timer.c sha2.c -D_PRINT_ -D_TABLE_
	./tvLED64-v2 > TV_tLED-v2-64.txt
	./tvLED96-v2 > TV_tLED-v2-96.txt
	./tvLED128-v2 > TV_tLED-v2-128.txt
	rm -rf tvLED64-v2  tvLED96-v2 tvLED128-v2
