# Phase 3 — Symmetric encryption with AES-128-CTR

[Notebook](../../README.md) · [Setup and configuration](../../docs/SETUP.md) · [Progress record](../../docs/PROGRESS.md)

**TEST KEY — NOT FOR PRODUCTION. AES-CTR here is an educational example without authentication. It must not replace AEAD in the Wings packet-security design.**

## 1. What are we trying to learn?

Encrypt readable text, decrypt it with the same key, and verify that the recovered bytes exactly match the original.

```text
Plaintext + test key + random IV
                ↓
           AES-128-CTR
                ↓
          IV | ciphertext
                ↓
     Decrypt using the same key
                ↓
        Recovered plaintext
```

Everything happens on the nRF5340. The PC is only a serial display at this stage.

## 2. The concepts in simple words

| Term | Meaning |
| --- | --- |
| Plaintext | Original readable bytes |
| Ciphertext | Encrypted bytes |
| Symmetric key | The same secret value is used to encrypt and decrypt |
| AES-128 | AES with a 128-bit, or 16-byte, key |
| CTR | Counter mode, a standard way to apply AES to a message |
| IV | Initialization vector: the starting counter block for this CTR operation |
| PSA key identifier | A value used to refer to an imported key object; it is not the AES key itself |

PSA's single-part encryption API generates a random 16-byte IV and prepends it to the ciphertext. Decryption needs that same IV, so it takes the complete `IV | ciphertext` buffer. The IV is not secret. CTR must not reuse counter-block values under the same key; fresh random IVs are used for this small experiment. Production limits and nonce/session design are future topics.

CTR encryption aims to hide data from someone without the key. It does not detect changes to the ciphertext. A decrypt call returning zero does not establish that the message is authentic. We will use AEAD later for that property. See the [PSA cipher API and its authentication guidance](https://arm-software.github.io/psa-api/crypto/1.1/api/ops/ciphers.html).

## 3. Files and configuration

- [src/main.c](src/main.c): complete example and diagnostic output.
- [prj.conf](prj.conf): shared console/PSA/random/memory settings plus `CONFIG_PSA_WANT_ALG_CTR=y`.
- [CMakeLists.txt](CMakeLists.txt): builds `wings_crypto_phase3` independently.

`CONFIG_PSA_WANT_ALG_CTR` requests AES-CTR support. The SDK automatically derives the internal `CONFIG_PSA_WANT_KEY_TYPE_AES` setting. All shared settings are explained in [setup](../../docs/SETUP.md#every-configuration-choice-in-these-lessons).

This build uses **Oberon software for AES-CTR** and the **CryptoCell platform RNG for the IV**. These choices were checked in the resolved configuration and compiled call paths. We are not claiming hardware AES-CTR acceleration in this lesson.

## 4. Read the code in this order

### The fixed test key and message

```c
/* TEST KEY - NOT FOR PRODUCTION. Public, fixed demonstration data. */
static const uint8_t test_key[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
static const uint8_t plaintext[] = "EMG test packet";
```

The public test key makes the lesson reproducible. Anyone with these bytes can decrypt the demonstration, so this is not real secrecy for application data.

### Initialize and import

`psa_crypto_init()` initializes the service. Key attributes specify AES, 128 bits, CTR, encryption/decryption permissions, and a volatile lifetime. `psa_import_key()` loads the test bytes into a PSA key object and returns its identifier.

Volatile means the PSA key object is not persisted across resets. The fixed test bytes still remain in the firmware source/image. This does not demonstrate protected storage or hardware-isolated keys.

`psa_reset_key_attributes()` releases the temporary attributes; it does not destroy the imported key.

### Encrypt and decrypt

```c
status = psa_cipher_encrypt(key, PSA_ALG_CTR,
                            plaintext, plaintext_length,
                            encrypted, sizeof(encrypted), &encrypted_length);

status = psa_cipher_decrypt(key, PSA_ALG_CTR,
                            encrypted, encrypted_length,
                            recovered, sizeof(recovered), &recovered_length);
```

These excerpts show the core calls; the complete file checks each result. The plaintext length excludes C's terminating zero byte. For the baseline, there are 15 plaintext bytes, 15 ciphertext bytes, and a separate 16-byte IV prefix in the 31-byte encryption output. No authentication tag exists.

`print_hex()` handles display formatting separately from the crypto calls. It does not implement encryption.

### Check and clean up

The example checks the recovered length and uses `memcmp()` to compare every recovered byte against the original. That is a local correctness check. A real receiver usually does not have the original plaintext for comparison; this check does not substitute for authentication.

`goto cleanup` routes error paths to `psa_destroy_key()` so the imported key object is released even after an operation fails. Destroying this object does not erase the fixed test-key bytes from the firmware image.

## 5. Build, flash, and view

From the **cryptocell root**:

```sh
./lesson.sh build 3
./lesson.sh flash 3
./lesson.sh ports
./lesson.sh monitor /dev/cu.usbmodem0010500464391
```

Select the current DK port and press RESET. In the UI, use the `03_aes_ctr` application and board `nrf5340dk_nrf5340_cpuapp`.

## 6. Expected output

```text
Wings crypto - Phase 3: AES-128-CTR
TEST KEY - NOT FOR PRODUCTION
Education only: encryption without authentication
psa_crypto_init: 0
psa_import_key: 0
Plaintext: EMG test packet
psa_cipher_encrypt: 0
IV (16 bytes): <32 hexadecimal characters>
Ciphertext (15 bytes): <30 hexadecimal characters>
psa_cipher_decrypt: 0
Recovered: EMG test packet
Exact match: YES
psa_destroy_key: 0
Phase 3 complete
```

The angle-bracket fields are explanatory placeholders; the device prints real hex bytes. Their values vary with the random IV. The baseline recovered message must match exactly.

## 7. Two experiments

1. Reset twice. Expect different IVs and ciphertexts, but the same recovered message. This connects Phase 1's random generation to encryption.
2. Change the plaintext to `EMG test packet!`, save, rebuild, and flash. Ciphertext length should become 16 bytes, and recovered text should include the exclamation mark. Restore the baseline string afterward.

Unlike SHA-256's fixed 32-byte result, CTR ciphertext length follows plaintext length. The separate IV adds transport overhead.

## 8. Stop and record the result

Record the device output in [the progress record](../../docs/PROGRESS.md). Look for successful encryption, decryption, cleanup, and `Exact match: YES`. Build success alone does not confirm these runtime checks.

**Supervisor explanation:** “We demonstrated reversible symmetric encryption through PSA. The same key and IV recover the original message. CTR does not authenticate data, so the next phase will introduce AEAD before we design secure EMG packets.”
