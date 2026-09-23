#include <stdint.h>
#include <psa/crypto.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define RANDOM_BYTE_COUNT 16

int main(void)
{
	uint8_t random_bytes[RANDOM_BYTE_COUNT];
	psa_status_t status;

	printk("\nWings crypto - Phase 1\n");
	status = psa_crypto_init();
	printk("psa_crypto_init: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		printk("FAIL: PSA initialization\n");
		return 0;
	}

	status = psa_generate_random(random_bytes, sizeof(random_bytes));
	printk("psa_generate_random: %d\n", (int)status);
	if (status != PSA_SUCCESS) {
		printk("FAIL: random generation\n");
		return 0;
	}

	printk("Random bytes (%u): ", (unsigned int)sizeof(random_bytes));
	for (size_t i = 0; i < sizeof(random_bytes); ++i) {
		printk("%02x", (unsigned int)random_bytes[i]);
	}
	printk("\nPhase 1 complete\n");
	return 0;
}
