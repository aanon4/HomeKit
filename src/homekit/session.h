/*
 * session.h
 *
 *  Created on: Jun 25, 2015
 *      Author: tim
 */

#ifndef HOMEKIT_SESSION_H_
#define HOMEKIT_SESSION_H_

typedef struct
{
  uint8_t     pve[64];

  // Shared secret established between client and accessory
  uint8_t     shared[32];

  // Keys and nonces used during encrypted comms
  struct
  {
    uint8_t   read[32];
    uint8_t   write[32];
    uint8_t   read_nonce[8];
    uint8_t   write_nonce[8];
  } transport;

  // DH keys
  struct
  {
    uint8_t   secret[32];
    uint8_t   public[32];
  } verify;

  // Client keys
  struct
  {
    uint8_t   public[32];
  } client;
} session_keys_t;

extern session_keys_t session_keys;

// Calculate the size of the cipher buffer based no the plaintext length (16 bytes of overhead)
#define SESSION_CIPHER_BUFFERLEN(PLAINLEN)  ((PLAINLEN) + 16)
#define SESSION_PLAIN_BUFFERLEN(CIPHERLEN)  ((CIPHERLEN) - 16)

extern void session_init(void);
extern void session_setEncryption(uint8_t enable);
extern uint8_t session_isEncrypted(void);
extern void session_readData(uint8_t* plaintext, uint16_t length, uint8_t* ciphertext, uint16_t* clength);
extern uint8_t session_writeData(uint8_t* ciphertext, uint16_t length, uint8_t* plaintext, uint16_t* plength);

#endif /* HOMEKIT_SESSION_H_ */
