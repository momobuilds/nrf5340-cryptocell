#ifndef WINGS_LESSON4_DEMO_CRYPTO_H
#define WINGS_LESSON4_DEMO_CRYPTO_H

#include <psa/crypto.h>

#define DEMO_NONCE_SIZE 13
#define DEMO_TAG_SIZE 16 /* PSA_ALG_CCM's default tag size. */

/* Call psa_crypto_init() first; caller destroys the returned volatile key. */
psa_status_t demo_import_test_key(psa_key_id_t *key);

/* Generates a fresh random nonce; output is ciphertext followed by tag.
 * Random nonces are for this small experiment, not a production nonce policy.
 */
psa_status_t demo_encrypt(psa_key_id_t key, uint8_t nonce[DEMO_NONCE_SIZE],
	const uint8_t *aad, size_t aad_length,
	const uint8_t *plaintext, size_t plaintext_length,
	uint8_t *output, size_t output_size, size_t *output_length);

/* Only consume output on PSA_SUCCESS. On failure output is cleared and its
 * reported length is zero. Arguments must point to valid caller buffers.
 */
psa_status_t demo_decrypt(psa_key_id_t key, const uint8_t nonce[DEMO_NONCE_SIZE],
	const uint8_t *aad, size_t aad_length,
	const uint8_t *input, size_t input_length,
	uint8_t *output, size_t output_size, size_t *output_length);

#endif
