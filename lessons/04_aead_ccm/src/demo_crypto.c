#include <string.h>
#include "demo_crypto.h"

psa_status_t demo_import_test_key(psa_key_id_t *key)
{
	/* TEST KEY - NOT FOR PRODUCTION. Public key bytes, not a real secret. */
	static const uint8_t test_key[16] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
	};
	psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
	psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
	psa_set_key_bits(&attributes, 128);
	psa_set_key_algorithm(&attributes, PSA_ALG_CCM);
	psa_set_key_usage_flags(&attributes,
		PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
	psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
	psa_status_t status = psa_import_key(&attributes, test_key, sizeof(test_key), key);
	psa_reset_key_attributes(&attributes);
	return status;
}

psa_status_t demo_encrypt(psa_key_id_t key, uint8_t nonce[DEMO_NONCE_SIZE],
	const uint8_t *aad, size_t aad_length,
	const uint8_t *plaintext, size_t plaintext_length,
	uint8_t *output, size_t output_size, size_t *output_length)
{
	*output_length = 0;
	psa_status_t status = psa_generate_random(nonce, DEMO_NONCE_SIZE);
	if (status != PSA_SUCCESS) {
		return status;
	}
	return psa_aead_encrypt(key, PSA_ALG_CCM, nonce, DEMO_NONCE_SIZE,
		aad, aad_length, plaintext, plaintext_length,
		output, output_size, output_length);
}

psa_status_t demo_decrypt(psa_key_id_t key, const uint8_t nonce[DEMO_NONCE_SIZE],
	const uint8_t *aad, size_t aad_length,
	const uint8_t *input, size_t input_length,
	uint8_t *output, size_t output_size, size_t *output_length)
{
	*output_length = 0;
	psa_status_t status = psa_aead_decrypt(key, PSA_ALG_CCM,
		nonce, DEMO_NONCE_SIZE, aad, aad_length,
		input, input_length, output, output_size, output_length);
	if (status != PSA_SUCCESS) {
		memset(output, 0, output_size);
		*output_length = 0;
	}
	return status;
}
