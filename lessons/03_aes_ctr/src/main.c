#include <stdint.h>
#include <string.h>
#include <psa/crypto.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static void print_hex(const char *label, const uint8_t *data, size_t length)
{
	printk("%s (%u bytes): ", label, (unsigned int)length);
	for (size_t i = 0; i < length; ++i) {
		printk("%02x", (unsigned int)data[i]);
	}
	printk("\n");
}

int main(void)
{
	/* TEST KEY - NOT FOR PRODUCTION. Public, fixed demonstration data. */
	static const uint8_t test_key[16] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
	};
	static const uint8_t plaintext[] = "EMG test packet";
	uint8_t encrypted[PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(
		PSA_KEY_TYPE_AES, PSA_ALG_CTR, sizeof(plaintext) - 1)];
	uint8_t recovered[sizeof(plaintext)];
	const size_t plaintext_length = sizeof(plaintext) - 1;
	const size_t iv_length = PSA_CIPHER_IV_LENGTH(PSA_KEY_TYPE_AES, PSA_ALG_CTR);
	size_t encrypted_length = 0;
	size_t recovered_length = 0;
	psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
	psa_key_id_t key = 0;
	psa_status_t status;
	psa_status_t destroy_status;

	printk("\nWings crypto - Phase 3: AES-128-CTR\n");
	printk("TEST KEY - NOT FOR PRODUCTION\n");
	printk("Education only: encryption without authentication\n");

	status = psa_crypto_init();
	printk("psa_crypto_init: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		printk("FAIL: PSA initialization\n");
		return 0;
	}

	psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
	psa_set_key_bits(&attributes, 128);
	psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
	psa_set_key_usage_flags(&attributes,
			      PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
	psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
	status = psa_import_key(&attributes, test_key, sizeof(test_key), &key);
	psa_reset_key_attributes(&attributes);
	printk("psa_import_key: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		printk("FAIL: key import\n");
		return 0;
	}

	printk("Plaintext: %s\n", (const char *)plaintext);
	/* PSA generates a random IV and prepends it to the ciphertext. */
	status = psa_cipher_encrypt(key, PSA_ALG_CTR,
				    plaintext, plaintext_length,
				    encrypted, sizeof(encrypted), &encrypted_length);
	printk("psa_cipher_encrypt: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		goto cleanup;
	}
	if (encrypted_length != iv_length + plaintext_length) {
		printk("FAIL: unexpected AES-CTR output length\n");
		status = PSA_ERROR_GENERIC_ERROR;
		goto cleanup;
	}
	print_hex("IV", encrypted, iv_length);
	print_hex("Ciphertext", encrypted + iv_length, encrypted_length - iv_length);

	/* Pass the complete IV | ciphertext buffer back to PSA. */
	status = psa_cipher_decrypt(key, PSA_ALG_CTR,
				    encrypted, encrypted_length,
				    recovered, sizeof(recovered), &recovered_length);
	printk("psa_cipher_decrypt: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		goto cleanup;
	}

	/* Local correctness check only. CTR does not authenticate received data. */
	if (recovered_length != plaintext_length ||
	    memcmp(recovered, plaintext, plaintext_length) != 0) {
		printk("FAIL: recovered bytes differ\n");
		status = PSA_ERROR_GENERIC_ERROR;
		goto cleanup;
	}
	printk("Recovered: %.*s\n", (int)recovered_length, (const char *)recovered);
	printk("Exact match: YES\n");

cleanup:
	/* Release the imported PSA key even if a later operation failed. */
	destroy_status = psa_destroy_key(key);
	printk("psa_destroy_key: %d\n", (int)destroy_status);
	if (status == PSA_SUCCESS && destroy_status == PSA_SUCCESS) {
		printk("Phase 3 complete\n");
	} else {
		printk("Phase 3 FAILED\n");
	}
	return 0;
}
