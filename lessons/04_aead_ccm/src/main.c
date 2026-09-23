#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include "demo_crypto.h"

static void print_hex(const char *label, const uint8_t *bytes, size_t length)
{
	printk("%s (%u bytes): ", label, (unsigned int)length);
	for (size_t i = 0; i < length; ++i) {
		printk("%02x", (unsigned int)bytes[i]);
	}
	printk("\n");
}

int main(void)
{
	static const uint16_t samples[] = {
		2010, 2034, 1995, 2051, 2070, 2028, 2001, 2040
	};
	/* Tiny teaching header: version=1, type=1 (EMG_RAW),
	 * sequence=1 (32-bit little endian), sample_count=8.
	 * AAD is visible but authenticated. This is not the final Wings format.
	 */
	const uint8_t aad[] = {1, 1, 1, 0, 0, 0, ARRAY_SIZE(samples)};
	uint8_t plaintext[2 * ARRAY_SIZE(samples)];
	uint8_t nonce[DEMO_NONCE_SIZE];
	uint8_t encrypted[sizeof(plaintext) + DEMO_TAG_SIZE];
	uint8_t recovered[sizeof(plaintext)];
	size_t encrypted_length = 0;
	size_t recovered_length = 0;
	psa_key_id_t key = 0;
	psa_status_t status;
	psa_status_t destroy_status;

	printk("\nWings crypto - Phase 4: AES-128-CCM\n");
	printk("TEST KEY - NOT FOR PRODUCTION\n");
	status = psa_crypto_init();
	printk("psa_crypto_init: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		return 0;
	}
	status = demo_import_test_key(&key);
	printk("psa_import_key: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		return 0;
	}

	/* Explicit byte encoding, not the memory layout of a C struct. */
	for (size_t i = 0; i < ARRAY_SIZE(samples); ++i) {
		sys_put_le16(samples[i], &plaintext[2 * i]);
	}
	printk("Header: version=1 type=EMG_RAW sequence=1 count=%u\n",
		(unsigned int)ARRAY_SIZE(samples));
	print_hex("AAD", aad, sizeof(aad));
	print_hex("Plaintext samples", plaintext, sizeof(plaintext));

	status = demo_encrypt(key, nonce, aad, sizeof(aad),
		plaintext, sizeof(plaintext),
		encrypted, sizeof(encrypted), &encrypted_length);
	printk("AEAD encrypt (including nonce generation): %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		goto cleanup;
	}
	if (encrypted_length != sizeof(plaintext) + DEMO_TAG_SIZE) {
		status = PSA_ERROR_GENERIC_ERROR;
		printk("FAIL: unexpected encrypted length\n");
		goto cleanup;
	}
	print_hex("Nonce", nonce, sizeof(nonce));
	print_hex("Ciphertext", encrypted, sizeof(plaintext));
	print_hex("Authentication tag", encrypted + sizeof(plaintext), DEMO_TAG_SIZE);

	/* PSA authenticates using the key, nonce, AAD, ciphertext and tag.
	 * It does NOT receive our original plaintext to do that check.
	 */
	status = demo_decrypt(key, nonce, aad, sizeof(aad),
		encrypted, encrypted_length,
		recovered, sizeof(recovered), &recovered_length);
	printk("psa_aead_decrypt: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		printk("REJECTED: do not use the recovered buffer\n");
		goto cleanup;
	}
	printk("Authentication: PASS\n");
	/* Additional local test only, after authentication has succeeded. */
	if (recovered_length != sizeof(plaintext) ||
	    memcmp(plaintext, recovered, sizeof(plaintext)) != 0) {
		status = PSA_ERROR_GENERIC_ERROR;
		printk("FAIL: round-trip mismatch\n");
		goto cleanup;
	}
	print_hex("Recovered bytes", recovered, recovered_length);
	printk("Recovered samples:");
	for (size_t i = 0; i < ARRAY_SIZE(samples); ++i) {
		printk(" %u", (unsigned int)sys_get_le16(&recovered[2 * i]));
	}
	printk("\nExact match: YES\n");

cleanup:
	destroy_status = psa_destroy_key(key);
	printk("psa_destroy_key: %d\n", (int)destroy_status);
	printk("%s\n", status == PSA_SUCCESS && destroy_status == PSA_SUCCESS ?
		"Phase 4 complete" : "Phase 4 FAILED");
	return 0;
}
