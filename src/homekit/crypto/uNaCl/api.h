#define crypto_scalarmult                         crypto_scalarmult_curve25519
#define crypto_scalarmult_base                    crypto_scalarmult_curve25519_base
#define crypto_scalarmult_BYTES                   crypto_scalarmult_curve25519_BYTES
#define crypto_scalarmult_SCALARBYTES             crypto_scalarmult_curve25519_SCALARBYTES
#define crypto_scalarmult_curve25519_BYTES        32
#define crypto_scalarmult_curve25519_SCALARBYTES  32

extern int crypto_scalarmult_curve25519(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_scalarmult_curve25519_base(unsigned char *,const unsigned char *);
