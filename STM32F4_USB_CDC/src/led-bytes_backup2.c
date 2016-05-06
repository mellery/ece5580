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

	
	const uint64_t T0[16][16]=  {
{0x5adb5adb,0x5adb7e1a,0x5adbb5fc,0x5adba795,0x5adb24c1,0x5adb0000,0x5adbef27,0x5adb1269,0x5adbcbe6,0x5adbd98f,0x5adb913d,0x5adb6c73,0x5adb36a8,0x5adbfd4e,0x5adb48b2,0x5adb8354},
{0x7e1a5adb,0x7e1a7e1a,0x7e1ab5fc,0x7e1aa795,0x7e1a24c1,0x7e1a0000,0x7e1aef27,0x7e1a1269,0x7e1acbe6,0x7e1ad98f,0x7e1a913d,0x7e1a6c73,0x7e1a36a8,0x7e1afd4e,0x7e1a48b2,0x7e1a8354},
{0xb5fc5adb,0xb5fc7e1a,0xb5fcb5fc,0xb5fca795,0xb5fc24c1,0xb5fc0000,0xb5fcef27,0xb5fc1269,0xb5fccbe6,0xb5fcd98f,0xb5fc913d,0xb5fc6c73,0xb5fc36a8,0xb5fcfd4e,0xb5fc48b2,0xb5fc8354},
{0xa7955adb,0xa7957e1a,0xa795b5fc,0xa795a795,0xa79524c1,0xa7950000,0xa795ef27,0xa7951269,0xa795cbe6,0xa795d98f,0xa795913d,0xa7956c73,0xa79536a8,0xa795fd4e,0xa79548b2,0xa7958354},
{0x24c15adb,0x24c17e1a,0x24c1b5fc,0x24c1a795,0x24c124c1,0x24c10000,0x24c1ef27,0x24c11269,0x24c1cbe6,0x24c1d98f,0x24c1913d,0x24c16c73,0x24c136a8,0x24c1fd4e,0x24c148b2,0x24c18354},
{0x00005adb,0x00007e1a,0x0000b5fc,0x0000a795,0x000024c1,0x00000000,0x0000ef27,0x00001269,0x0000cbe6,0x0000d98f,0x0000913d,0x00006c73,0x000036a8,0x0000fd4e,0x000048b2,0x00008354},
{0xef275adb,0xef277e1a,0xef27b5fc,0xef27a795,0xef2724c1,0xef270000,0xef27ef27,0xef271269,0xef27cbe6,0xef27d98f,0xef27913d,0xef276c73,0xef2736a8,0xef27fd4e,0xef2748b2,0xef278354},
{0x12695adb,0x12697e1a,0x1269b5fc,0x1269a795,0x126924c1,0x12690000,0x1269ef27,0x12691269,0x1269cbe6,0x1269d98f,0x1269913d,0x12696c73,0x126936a8,0x1269fd4e,0x126948b2,0x12698354},
{0xcbe65adb,0xcbe67e1a,0xcbe6b5fc,0xcbe6a795,0xcbe624c1,0xcbe60000,0xcbe6ef27,0xcbe61269,0xcbe6cbe6,0xcbe6d98f,0xcbe6913d,0xcbe66c73,0xcbe636a8,0xcbe6fd4e,0xcbe648b2,0xcbe68354},
{0xd98f5adb,0xd98f7e1a,0xd98fb5fc,0xd98fa795,0xd98f24c1,0xd98f0000,0xd98fef27,0xd98f1269,0xd98fcbe6,0xd98fd98f,0xd98f913d,0xd98f6c73,0xd98f36a8,0xd98ffd4e,0xd98f48b2,0xd98f8354},
{0x913d5adb,0x913d7e1a,0x913db5fc,0x913da795,0x913d24c1,0x913d0000,0x913def27,0x913d1269,0x913dcbe6,0x913dd98f,0x913d913d,0x913d6c73,0x913d36a8,0x913dfd4e,0x913d48b2,0x913d8354},
{0x6c735adb,0x6c737e1a,0x6c73b5fc,0x6c73a795,0x6c7324c1,0x6c730000,0x6c73ef27,0x6c731269,0x6c73cbe6,0x6c73d98f,0x6c73913d,0x6c736c73,0x6c7336a8,0x6c73fd4e,0x6c7348b2,0x6c738354},
{0x36a85adb,0x36a87e1a,0x36a8b5fc,0x36a8a795,0x36a824c1,0x36a80000,0x36a8ef27,0x36a81269,0x36a8cbe6,0x36a8d98f,0x36a8913d,0x36a86c73,0x36a836a8,0x36a8fd4e,0x36a848b2,0x36a88354},
{0xfd4e5adb,0xfd4e7e1a,0xfd4eb5fc,0xfd4ea795,0xfd4e24c1,0xfd4e0000,0xfd4eef27,0xfd4e1269,0xfd4ecbe6,0xfd4ed98f,0xfd4e913d,0xfd4e6c73,0xfd4e36a8,0xfd4efd4e,0xfd4e48b2,0xfd4e8354},
{0x48b25adb,0x48b27e1a,0x48b2b5fc,0x48b2a795,0x48b224c1,0x48b20000,0x48b2ef27,0x48b21269,0x48b2cbe6,0x48b2d98f,0x48b2913d,0x48b26c73,0x48b236a8,0x48b2fd4e,0x48b248b2,0x48b28354},
{0x83545adb,0x83547e1a,0x8354b5fc,0x8354a795,0x835424c1,0x83540000,0x8354ef27,0x83541269,0x8354cbe6,0x8354d98f,0x8354913d,0x83546c73,0x835436a8,0x8354fd4e,0x835448b2,0x83548354},

	};
  	const uint64_t T1[16][16]=
	{
	{0xce4bce4b,0xce4b5d3a,0xce4b672c,0xce4bbf85,0xce4b9371,0xce4b0000,0xce4ba967,0xce4bd8a9,0xce4b3a16,0xce4be2bf,0xce4bf45d,0xce4b8593,0xce4b4bd8,0xce4b71ce,0xce4b16e2,0xce4b2cf4},
{0x5d3ace4b,0x5d3a5d3a,0x5d3a672c,0x5d3abf85,0x5d3a9371,0x5d3a0000,0x5d3aa967,0x5d3ad8a9,0x5d3a3a16,0x5d3ae2bf,0x5d3af45d,0x5d3a8593,0x5d3a4bd8,0x5d3a71ce,0x5d3a16e2,0x5d3a2cf4},
{0x672cce4b,0x672c5d3a,0x672c672c,0x672cbf85,0x672c9371,0x672c0000,0x672ca967,0x672cd8a9,0x672c3a16,0x672ce2bf,0x672cf45d,0x672c8593,0x672c4bd8,0x672c71ce,0x672c16e2,0x672c2cf4},
{0xbf85ce4b,0xbf855d3a,0xbf85672c,0xbf85bf85,0xbf859371,0xbf850000,0xbf85a967,0xbf85d8a9,0xbf853a16,0xbf85e2bf,0xbf85f45d,0xbf858593,0xbf854bd8,0xbf8571ce,0xbf8516e2,0xbf852cf4},
{0x9371ce4b,0x93715d3a,0x9371672c,0x9371bf85,0x93719371,0x93710000,0x9371a967,0x9371d8a9,0x93713a16,0x9371e2bf,0x9371f45d,0x93718593,0x93714bd8,0x937171ce,0x937116e2,0x93712cf4},
{0x0000ce4b,0x00005d3a,0x0000672c,0x0000bf85,0x00009371,0x00000000,0x0000a967,0x0000d8a9,0x00003a16,0x0000e2bf,0x0000f45d,0x00008593,0x00004bd8,0x000071ce,0x000016e2,0x00002cf4},
{0xa967ce4b,0xa9675d3a,0xa967672c,0xa967bf85,0xa9679371,0xa9670000,0xa967a967,0xa967d8a9,0xa9673a16,0xa967e2bf,0xa967f45d,0xa9678593,0xa9674bd8,0xa96771ce,0xa96716e2,0xa9672cf4},
{0xd8a9ce4b,0xd8a95d3a,0xd8a9672c,0xd8a9bf85,0xd8a99371,0xd8a90000,0xd8a9a967,0xd8a9d8a9,0xd8a93a16,0xd8a9e2bf,0xd8a9f45d,0xd8a98593,0xd8a94bd8,0xd8a971ce,0xd8a916e2,0xd8a92cf4},
{0x3a16ce4b,0x3a165d3a,0x3a16672c,0x3a16bf85,0x3a169371,0x3a160000,0x3a16a967,0x3a16d8a9,0x3a163a16,0x3a16e2bf,0x3a16f45d,0x3a168593,0x3a164bd8,0x3a1671ce,0x3a1616e2,0x3a162cf4},
{0xe2bfce4b,0xe2bf5d3a,0xe2bf672c,0xe2bfbf85,0xe2bf9371,0xe2bf0000,0xe2bfa967,0xe2bfd8a9,0xe2bf3a16,0xe2bfe2bf,0xe2bff45d,0xe2bf8593,0xe2bf4bd8,0xe2bf71ce,0xe2bf16e2,0xe2bf2cf4},
{0xf45dce4b,0xf45d5d3a,0xf45d672c,0xf45dbf85,0xf45d9371,0xf45d0000,0xf45da967,0xf45dd8a9,0xf45d3a16,0xf45de2bf,0xf45df45d,0xf45d8593,0xf45d4bd8,0xf45d71ce,0xf45d16e2,0xf45d2cf4},
{0x8593ce4b,0x85935d3a,0x8593672c,0x8593bf85,0x85939371,0x85930000,0x8593a967,0x8593d8a9,0x85933a16,0x8593e2bf,0x8593f45d,0x85938593,0x85934bd8,0x859371ce,0x859316e2,0x85932cf4},
{0x4bd8ce4b,0x4bd85d3a,0x4bd8672c,0x4bd8bf85,0x4bd89371,0x4bd80000,0x4bd8a967,0x4bd8d8a9,0x4bd83a16,0x4bd8e2bf,0x4bd8f45d,0x4bd88593,0x4bd84bd8,0x4bd871ce,0x4bd816e2,0x4bd82cf4},
{0x71cece4b,0x71ce5d3a,0x71ce672c,0x71cebf85,0x71ce9371,0x71ce0000,0x71cea967,0x71ced8a9,0x71ce3a16,0x71cee2bf,0x71cef45d,0x71ce8593,0x71ce4bd8,0x71ce71ce,0x71ce16e2,0x71ce2cf4},
{0x16e2ce4b,0x16e25d3a,0x16e2672c,0x16e2bf85,0x16e29371,0x16e20000,0x16e2a967,0x16e2d8a9,0x16e23a16,0x16e2e2bf,0x16e2f45d,0x16e28593,0x16e24bd8,0x16e271ce,0x16e216e2,0x16e22cf4},
{0x2cf4ce4b,0x2cf45d3a,0x2cf4672c,0x2cf4bf85,0x2cf49371,0x2cf40000,0x2cf4a967,0x2cf4d8a9,0x2cf43a16,0x2cf4e2bf,0x2cf4f45d,0x2cf48593,0x2cf44bd8,0x2cf471ce,0x2cf416e2,0x2cf42cf4},

	};
	const uint64_t T2[16][16]=
	{
	{0xb918b918,0xb918a246,0xb918cd94,0xb9185123,0xb9181b5e,0xb9180000,0xb918748c,0xb9189cb7,0xb9186fd2,0xb918f365,0xb918d6ca,0xb9183ef1,0xb91887e9,0xb918e83b,0xb91825af,0xb9184a7d},
{0xa246b918,0xa246a246,0xa246cd94,0xa2465123,0xa2461b5e,0xa2460000,0xa246748c,0xa2469cb7,0xa2466fd2,0xa246f365,0xa246d6ca,0xa2463ef1,0xa24687e9,0xa246e83b,0xa24625af,0xa2464a7d},
{0xcd94b918,0xcd94a246,0xcd94cd94,0xcd945123,0xcd941b5e,0xcd940000,0xcd94748c,0xcd949cb7,0xcd946fd2,0xcd94f365,0xcd94d6ca,0xcd943ef1,0xcd9487e9,0xcd94e83b,0xcd9425af,0xcd944a7d},
{0x5123b918,0x5123a246,0x5123cd94,0x51235123,0x51231b5e,0x51230000,0x5123748c,0x51239cb7,0x51236fd2,0x5123f365,0x5123d6ca,0x51233ef1,0x512387e9,0x5123e83b,0x512325af,0x51234a7d},
{0x1b5eb918,0x1b5ea246,0x1b5ecd94,0x1b5e5123,0x1b5e1b5e,0x1b5e0000,0x1b5e748c,0x1b5e9cb7,0x1b5e6fd2,0x1b5ef365,0x1b5ed6ca,0x1b5e3ef1,0x1b5e87e9,0x1b5ee83b,0x1b5e25af,0x1b5e4a7d},
{0x0000b918,0x0000a246,0x0000cd94,0x00005123,0x00001b5e,0x00000000,0x0000748c,0x00009cb7,0x00006fd2,0x0000f365,0x0000d6ca,0x00003ef1,0x000087e9,0x0000e83b,0x000025af,0x00004a7d},
{0x748cb918,0x748ca246,0x748ccd94,0x748c5123,0x748c1b5e,0x748c0000,0x748c748c,0x748c9cb7,0x748c6fd2,0x748cf365,0x748cd6ca,0x748c3ef1,0x748c87e9,0x748ce83b,0x748c25af,0x748c4a7d},
{0x9cb7b918,0x9cb7a246,0x9cb7cd94,0x9cb75123,0x9cb71b5e,0x9cb70000,0x9cb7748c,0x9cb79cb7,0x9cb76fd2,0x9cb7f365,0x9cb7d6ca,0x9cb73ef1,0x9cb787e9,0x9cb7e83b,0x9cb725af,0x9cb74a7d},
{0x6fd2b918,0x6fd2a246,0x6fd2cd94,0x6fd25123,0x6fd21b5e,0x6fd20000,0x6fd2748c,0x6fd29cb7,0x6fd26fd2,0x6fd2f365,0x6fd2d6ca,0x6fd23ef1,0x6fd287e9,0x6fd2e83b,0x6fd225af,0x6fd24a7d},
{0xf365b918,0xf365a246,0xf365cd94,0xf3655123,0xf3651b5e,0xf3650000,0xf365748c,0xf3659cb7,0xf3656fd2,0xf365f365,0xf365d6ca,0xf3653ef1,0xf36587e9,0xf365e83b,0xf36525af,0xf3654a7d},
{0xd6cab918,0xd6caa246,0xd6cacd94,0xd6ca5123,0xd6ca1b5e,0xd6ca0000,0xd6ca748c,0xd6ca9cb7,0xd6ca6fd2,0xd6caf365,0xd6cad6ca,0xd6ca3ef1,0xd6ca87e9,0xd6cae83b,0xd6ca25af,0xd6ca4a7d},
{0x3ef1b918,0x3ef1a246,0x3ef1cd94,0x3ef15123,0x3ef11b5e,0x3ef10000,0x3ef1748c,0x3ef19cb7,0x3ef16fd2,0x3ef1f365,0x3ef1d6ca,0x3ef13ef1,0x3ef187e9,0x3ef1e83b,0x3ef125af,0x3ef14a7d},
{0x87e9b918,0x87e9a246,0x87e9cd94,0x87e95123,0x87e91b5e,0x87e90000,0x87e9748c,0x87e99cb7,0x87e96fd2,0x87e9f365,0x87e9d6ca,0x87e93ef1,0x87e987e9,0x87e9e83b,0x87e925af,0x87e94a7d},
{0xe83bb918,0xe83ba246,0xe83bcd94,0xe83b5123,0xe83b1b5e,0xe83b0000,0xe83b748c,0xe83b9cb7,0xe83b6fd2,0xe83bf365,0xe83bd6ca,0xe83b3ef1,0xe83b87e9,0xe83be83b,0xe83b25af,0xe83b4a7d},
{0x25afb918,0x25af246,0x25afcd94,0x25af5123,0x25af1b5e,0x25af0000,0x25af748c,0x25af9cb7,0x25af6fd2,0x25aff365,0x25afd6ca,0x25af3ef1,0x25af87e9,0x25afe83b,0x25af25af,0x25af4a7d},
{0x4a7db918,0x4a7da246,0x4a7dcd94,0x4a7d5123,0x4a7d1b5e,0x4a7d0000,0x4a7d748c,0x4a7d9cb7,0x4a7d6fd2,0x4a7df365,0x4a7dd6ca,0x4a7d3ef1,0x4a7d87e9,0x4a7de83b,0x4a7d25af,0x4a7d4a7d},
	};
	const uint64_t T3[16][16]={
	{0xbe6dbe6d,0xbe6dadb1,0xbe6dc73f,0xbe6d5fc9,0xbe6d13dc,0xbe6d0000,0xbe6d7952,0xbe6d98f6,0xbe6d6a8e,0xbe6df278,0xbe6dd4e3,0xbe6d3547,0xbe6d8b2a,0xbe6de1a4,0xbe6d269b,0xbe6d4c15},
{0xadb1be6d,0xadb1adb1,0xadb1c73f,0xadb15fc9,0xadb113dc,0xadb10000,0xadb17952,0xadb198f6,0xadb16a8e,0xadb1f278,0xadb1d4e3,0xadb13547,0xadb18b2a,0xadb1e1a4,0xadb1269b,0xadb14c15},
{0xc73fbe6d,0xc73fadb1,0xc73fc73f,0xc73f5fc9,0xc73f13dc,0xc73f0000,0xc73f7952,0xc73f98f6,0xc73f6a8e,0xc73ff278,0xc73fd4e3,0xc73f3547,0xc73f8b2a,0xc73fe1a4,0xc73f269b,0xc73f4c15},
{0x5fc9be6d,0x5fc9adb1,0x5fc9c73f,0x5fc95fc9,0x5fc913dc,0x5fc90000,0x5fc97952,0x5fc99af6,0x5fc96a8e,0x5fc9f278,0x5fc9d4e3,0x5fc93547,0x5fc98b2a,0x5fc9e1a4,0x5fc9269b,0x5fc94c15},
{0x13dcbe6d,0x13dcadb1,0x13dcc73f,0x13dc5fc9,0x13dc13dc,0x13dc0000,0x13dc7952,0x13dc9af6,0x13dc6a8e,0x13dcf278,0x13dcd4e3,0x13dc3547,0x13dc8b2a,0x13dce1a4,0x13dc269b,0x13dc4c15},
{0x0000be6d,0x0000adb1,0x0000c73f,0x00005fc9,0x000013dc,0x00000000,0x00007952,0x00009af6,0x00006a8e,0x0000f278,0x0000d4e3,0x00003547,0x00008b2a,0x0000e1a4,0x0000269b,0x00004c15},
{0x7952be6d,0x7952adb1,0x7952c73f,0x79525fc9,0x795213dc,0x79520000,0x79527952,0x79529af6,0x79526a8e,0x7952f278,0x7952d4e3,0x79523547,0x79528b2a,0x792e1a4,0x7952269b,0x79524c15},
{0x98f6be6d,0x98f6adb1,0x98f6c73f,0x98f65fc9,0x98f613dc,0x98f60000,0x98f67952,0x98f698f6,0x98f66a8e,0x98f6f278,0x98f6d4e3,0x98f63547,0x98f68b2a,0x98f6e1a4,0x98f6269b,0x98f64c15},
{0x6a8ebe6d,0x6a8eadb1,0x6a8ec73f,0x6a8e5fc9,0x6a8e13dc,0x6a8e0000,0x6a8e7952,0x6a8e98f6,0x6a8e6a8e,0x6a8ef278,0x6a8ed4e3,0x6a8e3547,0x6a8e8b2a,0x6a8ee1a4,0x6a8e269b,0x6a8e4c15},
{0xf278be6d,0xf278adb1,0xf278c73f,0xf2785fc9,0xf27813dc,0xf2780000,0xf2787952,0xf27898f6,0xf2786a8e,0xf278f278,0xf278d4e3,0xf2783547,0xf2788b2a,0xf278e1a4,0xf278269b,0xf2784c15},
{0xd4e3be6d,0xd4e3adb1,0xd4e3c73f,0xd4e35fc9,0xd4e313dc,0xd4e30000,0xd4e37952,0xd4e398f6,0xd4e36a8e,0xd4e3f278,0xd4e3d4e3,0xd4e33547,0xd4e38b2a,0xd4e3e1a4,0xd4e3269b,0xd4e34c15},
{0x3547be6d,0x3547adb1,0x3547c73f,0x35475fc9,0x354713dc,0x35470000,0x35477952,0x354798f6,0x35476a8e,0x3547f278,0x3547d4e3,0x35473547,0x35478b2a,0x3547e1a4,0x3547269b,0x35474c15},
{0x8b2abe6d,0x8b2aadb1,0x8b2ac73f,0x8b2a5fc9,0x8b2a13dc,0x8b2a0000,0x8b2a7952,0x8b2a98f6,0x8b2a6a8e,0x8b2af278,0x8b2ad4e3,0x8b2a3547,0x8b2a8b2a,0x8b2ae1a4,0x8b2a269b,0x8b2a4c15},
{0xe1a4be6d,0xe1a4adb1,0xe1a4c73f,0xe1a45fc9,0xe1a413dc,0xe1a40000,0xe1a47952,0xe1a498f6,0xe1a46a8e,0xe1a4f278,0xe1a4d4e3,0xe1a43547,0xe1a48b2a,0xe1a4e1a4,0xe1a4269b,0xe1a44c15},
{0x269bbe6d,0x269badb1,0x269bc73f,0x269b5fc9,0x269b13dc,0x269b0000,0x269b7952,0x269b98f6,0x269b6a8e,0x269bf278,0x269bd4e3,0x269b3547,0x269b8b2a,0x269be1a4,0x269b269b,0x269b4c15},
{0x4c15be6d,0x4c15adb1,0x4c15c73f,0x4c155fc9,0x4c1513dc,0x4c150000,0x4c157952,0x4c1598f6,0x4c156a8e,0x4c15f278,0x4c15d4e3,0x4c153547,0x4c158b2a,0x4c15e1a4,0x4c15269b,0x4c154c15},


	};
//void tbox( uint64_t  state[4][2])
void tbox( unsigned short   state[4][2])
{
	int j;
	for (j=0;j<2;j++)
	{
	     state[0][j] = T0[((state[0][j]) >> 4)&0xF][(state[0][j]) & 0xF];
	     state[1][j] = T1[((state[1][j]) >> 4)&0xF][(state[1][j]) & 0xF];
	     state[2][j] = T2[((state[2][j]) >> 4)&0xF][(state[2][j]) & 0xF];
	     state[3][j] = T3[((state[3][j]) >> 4)&0xF][(state[3][j]) & 0xF];
	     state[0][j]= state[0][j]^state[1][j];
	     state[0][j]^=state[2][j];
  	     state[0][j]^=state[3][j];
	}
	
}
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
}
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

}
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
void SubCellBytes_1col(unsigned short state[4])
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


}
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
void ShiftRowBytes_1col(unsigned short state[4])
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

/*** MixColumns ***/
//
//
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


void LED_enc_1col22(unsigned short* input, unsigned short* userkey, int ksbits)
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
}
void LED_enc_2col_tbox(unsigned short* input, const unsigned short* userkey, int ksbits)
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
			//SubCell_2col(state);
			//MixColumn_2col(state);
 			tbox(state);
			ShiftRow_2col(state);
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
	//print_state_2col(state);
	print_state16(state);
	
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

