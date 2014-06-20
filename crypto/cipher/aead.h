/* Copyright (c) 2014, Google Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */

#ifndef OPENSSL_HEADER_AEAD_H
#define OPENSSL_HEADER_AEAD_H

#include <openssl/base.h>

#if defined(__cplusplus)
extern "C" {
#endif


/* Authenticated Encryption with Additional Data.
 *
 * AEAD couples confidentiality and integrity in a single primtive. AEAD
 * algorithms take a key and then can seal and open individual messages. Each
 * message has a unique, per-message nonce and, optionally, additional data
 * which is authenticated but not included in the ciphertext.
 *
 * The |EVP_AEAD_CTX_init| function initialises an |EVP_AEAD_CTX| structure and
 * performs any precomputation needed to use |aead| with |key|. The length of
 * the key, |key_len|, is given in bytes.
 *
 * The |tag_len| argument contains the length of the tags, in bytes, and allows
 * for the processing of truncated authenticators. A zero value indicates that
 * the default tag length should be used and this is defined as
 * |EVP_AEAD_DEFAULT_TAG_LENGTH| in order to make the code clear. Using
 * truncated tags increases an attacker's chance of creating a valid forgery.
 * Be aware that the attacker's chance may increase more than exponentially as
 * would naively be expected.
 *
 * When no longer needed, the initialised |EVP_AEAD_CTX| structure must be
 * passed to |EVP_AEAD_CTX_cleanup|, which will deallocate any memory used.
 *
 * With an |EVP_AEAD_CTX| in hand, one can seal and open messages. These
 * operations are intended to meet the standard notions of privacy and
 * authenticity for authenticated encryption. For formal definitions see
 * Bellare and Namprempre, "Authenticated encryption: relations among notions
 * and analysis of the generic composition paradigm," Lecture Notes in Computer
 * Science B<1976> (2000), 531–545,
 * http://www-cse.ucsd.edu/~mihir/papers/oem.html.
 *
 * When sealing messages, a nonce must be given. The length of the nonce is
 * fixed by the AEAD in use and is returned by |EVP_AEAD_nonce_length|. *The
 * nonce must be unique for all messages with the same key*. This is critically
 * important - nonce reuse may completely undermine the security of the AEAD.
 * Nonces may be predictable and public, so long as they are unique. Uniqueness
 * may be achieved with a simple counter or, if large enough, may be generated
 * randomly. The nonce must be passed into the "open" operation by the receiver
 * so must either be implicit (e.g. a counter), or must be transmitted along
 * with the sealed message.
 *
 * The "seal" and "open" operations are atomic - an entire message must be
 * encrypted or decrypted in a single call. Large messages may have to be split
 * up in order to accomodate this. When doing so, be mindful of the need not to
 * repeat nonces and the possibility that an attacker could duplicate, reorder
 * or drop message chunks. For example, using a single key for a given (large)
 * message and sealing chunks with nonces counting from zero would be secure as
 * long as the number of chunks was securely transmitted. (Otherwise an
 * attacker could truncate the message by dropping chunks from the end.)
 *
 * The number of chunks could be transmitted by prefixing it to the plaintext,
 * for example. This also assumes that no other message would ever use the same
 * key otherwise the rule that nonces must be unique for a given key would be
 * violated.
 *
 * The "seal" and "open" operations also permit additional data to be
 * authenticated via the |ad| parameter. This data is not included in the
 * ciphertext and must be identical for both the "seal" and "open" call. This
 * permits implicit context to be authenticated but may be empty if not needed.
 *
 * The "seal" and "open" operations may work in-place if the |out| and |in|
 * arguments are equal. They may also be used to shift the data left inside the
 * same buffer if |out| is less than |in|. However, |out| may not point inside
 * the input data otherwise the input may be overwritten before it has been
 * read. This situation will cause an error.
 *
 * The "seal" and "open" operations return one on success and zero on error. */


/* AEAD algorithms. */

/* EVP_aes_128_gcm is AES-128 in Galois Counter Mode. */
const EVP_AEAD *EVP_aead_aes_128_gcm(void);

/* EVP_aes_256_gcm is AES-256 in Galois Counter Mode. */
const EVP_AEAD *EVP_aead_aes_256_gcm(void);

/* EVP_aead_chacha20_poly1305 is an AEAD built from ChaCha20 and Poly1305. */
const EVP_AEAD *EVP_aead_chacha20_poly1305();


/* Utility functions. */

/* EVP_AEAD_key_length returns the length, in bytes, of the keys used by
 * |aead|. */
size_t EVP_AEAD_key_length(const EVP_AEAD *aead);

/* EVP_AEAD_nonce_length returns the length, in bytes, of the per-message nonce
 * for |aead|. */
size_t EVP_AEAD_nonce_length(const EVP_AEAD *aead);

/* EVP_AEAD_max_overhead returns the maximum number of additional bytes added
 * by the act of sealing data with |aead|. */
size_t EVP_AEAD_max_overhead(const EVP_AEAD *aead);

/* EVP_AEAD_max_tag_len returns the maximum tag length when using |aead|. This
 * is the largest value that can be passed as |tag_len| to
 * |EVP_AEAD_CTX_init|. */
size_t EVP_AEAD_max_tag_len(const EVP_AEAD *aead);


/* AEAD operations. */

/* An EVP_AEAD_CTX represents an AEAD algorithm configured with a specific key
 * and message-independent IV. */
typedef struct evp_aead_ctx_st {
  const EVP_AEAD *aead;
  /* aead_state is an opaque pointer to whatever state the AEAD needs to
   * maintain. */
  void *aead_state;
} EVP_AEAD_CTX;

/* EVP_AEAD_MAX_OVERHEAD contains the maximum overhead used by any AEAD
 * defined in this header. */
#define EVP_AEAD_MAX_OVERHEAD 16

/* EVP_AEAD_DEFAULT_TAG_LENGTH is a magic value that can be passed to
 * EVP_AEAD_CTX_init to indicate that the default tag length for an AEAD should
 * be used. */
#define EVP_AEAD_DEFAULT_TAG_LENGTH 0

/* EVP_AEAD_init initializes |ctx| for the given AEAD algorithm from |impl|.
 * The |impl| argument may be NULL to choose the default implementation.
 * Authentication tags may be truncated by passing a size as |tag_len|. A
 * |tag_len| of zero indicates the default tag length and this is defined as
 * EVP_AEAD_DEFAULT_TAG_LENGTH for readability.
 * Returns 1 on success. Otherwise returns 0 and pushes to the error stack. */
int EVP_AEAD_CTX_init(EVP_AEAD_CTX *ctx, const EVP_AEAD *aead,
                      const uint8_t *key, size_t key_len, size_t tag_len,
                      ENGINE *impl);

/* EVP_AEAD_CTX_cleanup frees any data allocated by |ctx|. */
void EVP_AEAD_CTX_cleanup(EVP_AEAD_CTX *ctx);

/* EVP_AEAD_CTX_seal encrypts and authenticates |in_len| bytes from |in| and
 * authenticates |ad_len| bytes from |ad| and writes the result to |out|. It
 * returns one on success and zero otherwise.
 *
 * This function may be called (with the same |EVP_AEAD_CTX|) concurrently with
 * itself or |EVP_AEAD_CTX_open|.
 *
 * At most |max_out_len| bytes are written to |out| and, in order to ensure
 * success, |max_out_len| should be |in_len| plus the result of
 * |EVP_AEAD_overhead|. On successful return, |*out_len| is set to the actual
 * number of bytes written.
 *
 * The length of |nonce|, |nonce_len|, must be equal to the result of
 * |EVP_AEAD_nonce_length| for this AEAD.
 *
 * |EVP_AEAD_CTX_seal| never results in a partial output. If |max_out_len| is
 * insufficient, zero will be returned. (In this case, |*out_len| is set to
 * zero.)
 *
 * If |in| and |out| alias then |out| must be <= |in|. */
int EVP_AEAD_CTX_seal(const EVP_AEAD_CTX *ctx, uint8_t *out, size_t *out_len,
                      size_t max_out_len, const uint8_t *nonce,
                      size_t nonce_len, const uint8_t *in, size_t in_len,
                      const uint8_t *ad, size_t ad_len);

/* EVP_AEAD_CTX_open authenticates |in_len| bytes from |in| and |ad_len| bytes
 * from |ad| and decrypts at most |in_len| bytes into |out|. It returns one on
 * success and zero otherwise.
 *
 * This function may be called (with the same |EVP_AEAD_CTX|) concurrently with
 * itself or |EVP_AEAD_CTX_seal|.
 *
 * At most |in_len| bytes are written to |out|. In order to ensure success,
 * |max_out_len| should be at least |in_len|. On successful return, |*out_len|
 * is set to the the actual number of bytes written.
 *
 * The length of |nonce|, |nonce_len|, must be equal to the result of
 * |EVP_AEAD_nonce_length| for this AEAD.
 *
 * |EVP_AEAD_CTX_open| never results in a partial output. If |max_out_len| is
 * insufficient, zero will be returned. (In this case, |*out_len| is set to
 * zero.)
 *
 * If |in| and |out| alias then |out| must be <= |in|. */
int EVP_AEAD_CTX_open(const EVP_AEAD_CTX *ctx, uint8_t *out, size_t *out_len,
                      size_t max_out_len, const uint8_t *nonce,
                      size_t nonce_len, const uint8_t *in, size_t in_len,
                      const uint8_t *ad, size_t ad_len);


#if defined(__cplusplus)
}  /* extern C */
#endif

#endif  /* OPENSSL_HEADER_AEAD_H */
