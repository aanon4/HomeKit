/*
 * session.c
 *
 *  Created on: Jun 25, 2015
 *      Author: tim
 */

#include <stdint.h>
#include <string.h>

#include "session.h"
#include "crypto/crypto.h"

session_keys_t session_keys;

static struct
{
  uint8_t encrypting;
  uint8_t havekeys;
} session_state;

void session_init(void)
{
  random_create(session_keys.verify.secret, sizeof(session_keys.verify.secret));
  crypto_scalarmult_base(session_keys.verify.public, session_keys.verify.secret);
  session_keys.verify.secret[0] &= 248;
  session_keys.verify.secret[31] = (session_keys.verify.secret[31] & 127) | 64;
  session_state.encrypting = 0;
  session_state.havekeys = 0;
}

void session_setEncryption(uint8_t enable)
{
  if (session_state.encrypting != enable)
  {
    session_state.encrypting = enable;
    session_state.havekeys = 0;
  }
}

uint8_t session_isEncrypted(void)
{
  return session_state.encrypting;
}

static void session_genkeys(void)
{
  if (!session_state.havekeys)
  {
    session_state.havekeys = 1;
    uint8_t key[64];
    crypto_hkdf(key, "Control-Salt", 12, "Control-Read-Encryption-Key\001", 28, session_keys.shared, sizeof(session_keys.shared));
    memcpy(session_keys.transport.read, key, 32);
    crypto_hkdf(key, "Control-Salt", 12, "Control-Write-Encryption-Key\001", 29, session_keys.shared, sizeof(session_keys.shared));
    memcpy(session_keys.transport.write, key, 32);
    memset(session_keys.transport.read_nonce, 0, sizeof(session_keys.transport.read_nonce));
    memset(session_keys.transport.write_nonce, 0, sizeof(session_keys.transport.write_nonce));
  }
}

void session_readData(uint8_t* plaintext, uint16_t length, uint8_t* ciphertext, uint16_t* clength)
{
  if (session_state.encrypting)
  {
    session_genkeys();
    crypto_transportEncrypt(session_keys.transport.read, session_keys.transport.read_nonce, plaintext, length, ciphertext, clength);
  }
  else
  {
    memcpy(ciphertext, plaintext, length);
    *clength = length;
  }
}

uint8_t session_writeData(uint8_t* ciphertext, uint16_t length, uint8_t* plaintext, uint16_t* plength)
{
  if (session_state.encrypting)
  {
    session_genkeys();
    return crypto_transportDecrypt(session_keys.transport.write, session_keys.transport.write_nonce, ciphertext, length, plaintext, plength);
  }
  else
  {
    memcpy(ciphertext, plaintext, length);
    *plength = length;
    return 1;
  }
}
