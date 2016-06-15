#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#ifdef __cplusplus
extern "C" {
#endif

// crypto functions in ESP SDK
// most of it from hostapd

// md5_i.h
struct MD5Context {
  u32 buf[4];
  u32 bits[2];
  u8 in[64];
};

void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, unsigned char const *buf,
         unsigned len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);

/**
 * md5_vector - MD5 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash
 * Returns: 0 on success, -1 of failure
 */
int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac);


// aes.h

#define AES_BLOCK_SIZE 16

void * aes_encrypt_init(const u8 *key, size_t len);
void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);
void aes_encrypt_deinit(void *ctx);

void * aes_decrypt_init(const u8 *key, size_t len);
void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);
void aes_decrypt_deinit(void *ctx);


// helper to do decryption of AES 128
inline int do_aes128_decrypt(const u8 *key, const u8 *crypt, u8 *plain) {
  void *ctx = aes_decrypt_init(key, AES_BLOCK_SIZE);
  if (ctx == NULL)
    return -1;
  aes_decrypt(ctx, crypt, plain);
  aes_decrypt_deinit(ctx);
  return 0;
}


#ifdef __cplusplus
}
#endif

#endif
