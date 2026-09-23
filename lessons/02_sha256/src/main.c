#include <stdint.h>
#include <string.h>
#include <psa/crypto.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static psa_status_t print_sha256(const char *message)
{
	uint8_t hash[PSA_HASH_LENGTH(PSA_ALG_SHA_256)];
	size_t hash_length = 0;
	psa_status_t status;

	printk("\nMessage: %s\n", message);
	/* Exclude C's terminating zero byte from the message. */
	status = psa_hash_compute(PSA_ALG_SHA_256,
				  (const uint8_t *)message, strlen(message),
				  hash, sizeof(hash), &hash_length);
	printk("psa_hash_compute: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		printk("FAIL: SHA-256\n");
		return status;
	}

	printk("SHA-256 (%u bytes): ", (unsigned int)hash_length);
	for (size_t i = 0; i < hash_length; ++i) {
		printk("%02x", (unsigned int)hash[i]);
	}
	printk("\n");
	return PSA_SUCCESS;
}

int main(void)
{
	psa_status_t status;

	printk("\nWings crypto - Phase 2: SHA-256\n");
	status = psa_crypto_init();
	printk("psa_crypto_init: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		printk("FAIL: PSA initialization\n");
		return 0;
	}

	if (print_sha256("EMG test packet") != PSA_SUCCESS) {
		return 0;
	}
	/* The baseline changes just one character: p -> P. */
	if (print_sha256("EMG test Packet") != PSA_SUCCESS) {
		return 0;
	}
	printk("\nPhase 2 complete\n");
	return 0;
}
