

#ifndef LED_TABLE_H
#define LED_TABLE_H

void LED128table_core(const uint8_t * plaintext, const uint8_t* roundKeys128, uint8_t* ciphertext);

void LED64table_core(const uint8_t* plaintext, const uint8_t* roundKeys64, uint8_t* ciphertext);

#endif
