# Phase 2 — SHA-256 hashing

[Notebook](../../README.md) · [Setup and configuration](../../docs/SETUP.md) · [Progress record](../../docs/PROGRESS.md)

## 1. What are we trying to learn?

Calculate the SHA-256 fingerprint of a message, then change one character and calculate it again.

```text
EMG test packet → SHA-256 → 32-byte hash
EMG test Packet → SHA-256 → different 32-byte hash
         p → P
```

## 2. The concept in simple words

Hashing creates a fixed-size fingerprint. SHA-256 always outputs 256 bits, which are 32 bytes or 64 hexadecimal characters. The input message can be shorter or longer.

- The same exact bytes always produce the same hash. No fresh randomness is involved in calculating this fingerprint.
- Changing a character normally changes many hash bits. This example has two different hashes.
- There is no secret key and no decryption operation. Hashing does not hide the message you send alongside it.
- A hash can detect a changed message when checked against a trusted reference. If an attacker can replace both the message and its plain hash, that check does not establish authenticity.

Calling a hash a fingerprint is an analogy, not a promise that different inputs can never share a hash. Collisions exist mathematically; SHA-256 is designed to make finding them computationally infeasible.

## 3. Files and configuration

- [src/main.c](src/main.c): initialize PSA and hash two strings.
- [prj.conf](prj.conf): the shared console/PSA/memory settings plus `CONFIG_PSA_WANT_ALG_SHA_256=y`.
- [CMakeLists.txt](CMakeLists.txt): builds the independent `wings_crypto_phase2` project.

SHA-256 was already enabled by SDK defaults; the explicit request documents our requirement. Every shared setting is explained in [setup](../../docs/SETUP.md#every-configuration-choice-in-these-lessons).

This build selects **Oberon software SHA-256**, verified in the generated configuration and compiled call path. Hardware random-generation support does not imply hardware SHA-256 is selected.

## 4. Read the code

The helper `print_sha256()` performs this operation and then prints the bytes:

```c
status = psa_hash_compute(PSA_ALG_SHA_256,
                          (const uint8_t *)message, strlen(message),
                          hash, sizeof(hash), &hash_length);
```

Arguments, in order:

1. Which hash algorithm to use.
2. Input bytes and their length.
3. Output buffer and its capacity.
4. Where PSA should report the output length.

`strlen()` excludes the C string's terminating zero byte. A space, newline, uppercase letter, or extra zero byte changes the input and therefore can change the hash. This exact-byte agreement matters for later interoperability.

The complete source checks the return status before reading the output. The API is documented in the [PSA hashing specification](https://arm-software.github.io/psa-api/crypto/1.1/api/ops/hashes.html); the signature was also checked in the installed SDK headers.

## 5. Build, flash, and view

From the **cryptocell root**:

```sh
./lesson.sh build 2
./lesson.sh flash 2
./lesson.sh ports
./lesson.sh monitor /dev/cu.usbmodem0010500464391
```

Use the current DK port and press RESET. In the UI, select the `02_sha256` application with board `nrf5340dk_nrf5340_cpuapp`. Each lesson has its own build, so switching to this one does not modify Phase 1.

## 6. Expected output

```text
Wings crypto - Phase 2: SHA-256
psa_crypto_init: 0

Message: EMG test packet
psa_hash_compute: 0
SHA-256 (32 bytes): 827ded6e4e2ed77b5820854a45c67190216cf8118b3274cb700a160cf919f63f

Message: EMG test Packet
psa_hash_compute: 0
SHA-256 (32 bytes): a4fb1e6e7fe696be2f1a60a1db7413f2e6005764bf715c0aad0c0b324f7d3680

Phase 2 complete
```

These are exact reference hashes independently calculated with Python's standard `hashlib`. With the baseline strings, your board should match both. A terminal may wrap the long lines.

## 7. Two experiments

1. Reset without editing. The hashes should be identical to the previous run.
2. Change only the second string to `EMG test Packet!!`, save, build, and flash. You tried this variation earlier. Expect a new hash of the same 32-byte length. Restore `EMG test Packet` to match the reference above.

## 8. Stop and record the result

Record both zero statuses and the exact digest comparison in [the progress record](../../docs/PROGRESS.md).

**Supervisor explanation:** “SHA-256 gives a reproducible fingerprint. This demonstrates change detection against a trusted reference, but it is neither encryption nor message authentication by itself.”
