/* 
 *	Copyright (c) 2014. All rights are reserved by Jian Guo.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL WE
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, OR CONSEQUENTIAL 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OF THE 
 * SOFTWARE.
 * 
 * Contact: Jian Guo, ntu.guo@gmail.com
 */

#ifndef LED_BYTES_2C_H
#define LED_BYTES_2C_H

void AddKey16(unsigned short state[4][2], unsigned short* keyBytes, int step);
void AddConstants16(unsigned short state[4][2], int r);
void SubCell16(unsigned short state[2][2]);
void ShiftRow16(unsigned short state[4][2]);
void MixColumn16(unsigned short state[4][2]);
void LED_enc16(unsigned short* input, const unsigned short* userkey, int ksbits);

#endif
