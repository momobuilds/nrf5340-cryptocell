# Phase 1 — Secure random bytes

[Notebook](../../README.md) · [Setup and configuration](../../docs/SETUP.md) · [Progress record](../../docs/PROGRESS.md)

## 1. What are we trying to learn?

Check whether the nRF5340 application can initialize PSA Crypto and obtain 16 secure random bytes. This is a small integration check before using other crypto APIs.

## 2. The concept in simple words

A secure random generator supplies bytes that should be unpredictable. These bytes are not an encrypted message. Here we print them and do nothing else with them.

Secure random generation can later be used to create secret keys. These printed demonstration bytes must not become production secrets. When we reach key management, PSA's key-generation API can manage a generated key through an identifier.

```text
Application calls PSA Crypto                    software
              ↓
Nordic security integration / PSA core           software
              ↓
Nordic CC312 platform random-generation library  software
              ↓
CryptoCell-312 entropy and generator support     hardware
```

The compiled configuration routes RNG calls to `nrf_cc3xx_platform_ctr_drbg_get()`. The installed library's header documents a hardware-accelerated cryptographic generator seeded/reseeded from hardware entropy. This is the selected implementation; a successful device run is the next verification step.

Looking at different-looking output is a sanity check, not a statistical or cryptographic proof of random quality. We rely on the supported SDK implementation, not a generator written by us.

## 3. Files and configuration

- [src/main.c](src/main.c): all application code for this lesson.
- [prj.conf](prj.conf): console, PSA/random support, working memory, and timing settings.
- [CMakeLists.txt](CMakeLists.txt): builds this lesson's `main.c` as `wings_crypto_phase1`.

The key settings are `CONFIG_NRF_SECURITY=y`, `CONFIG_MBEDTLS_PSA_CRYPTO_C=y`, and `CONFIG_PSA_WANT_GENERATE_RANDOM=y`. The [configuration table](../../docs/SETUP.md#every-configuration-choice-in-these-lessons) explains every setting, including memory and console options.

## 4. Read the code

```c
status = psa_crypto_init();
status = psa_generate_random(random_bytes, sizeof(random_bytes));
```

These are the two central operations, shown without the surrounding checks here. The complete source checks the status after each call and stops on failure. `PSA_SUCCESS` is zero.

- `uint8_t random_bytes[16]` reserves space for 16 bytes.
- `sizeof(random_bytes)` passes the buffer size to PSA.
- `%02x` prints a byte as two hexadecimal characters, so 16 bytes produce 32 characters.
- Returning from `main()` ends this lesson's main-thread work. The board does not need to be flashed again to rerun it; reset is enough.

## 5. Build, flash, and view

From the **cryptocell root**, not from this lesson folder:

```sh
./lesson.sh build 1
./lesson.sh flash 1
./lesson.sh ports
./lesson.sh monitor /dev/cu.usbmodem0010500464391
```

Use your current DK port. Press RESET after the monitor opens. The build target is `nrf5340dk_nrf5340_cpuapp`. For the UI, choose the `01_random` application and that same board. See [setup](../../docs/SETUP.md#using-the-nordic-vs-code-ui).

## 6. Expected output

```text
Wings crypto - Phase 1
psa_crypto_init: 0
psa_generate_random: 0
Random bytes (16): 7b20e4c96a01d853bf442e908d17c5aa
Phase 1 complete
```

The random value above is illustrative. Your bytes should differ; the status lines should match. A Zephyr boot banner may appear first.

## 7. Two experiments

1. Press RESET twice. Expect different random output without rebuilding.
2. Change `RANDOM_BYTE_COUNT` to `32`, save, rebuild, and flash. Expect 64 hexadecimal characters. Restore `16` when returning to the baseline.

Neither experiment encrypts anything.

## 8. Stop and record the result

Record the output and both zero status values in [the progress record](../../docs/PROGRESS.md). If initialization or generation fails, resolve that before progressing.

**Supervisor explanation:** “This tests our application-to-PSA random-generation path on the nRF5340. Random bytes are useful building blocks for future keys, but this demonstration does not encrypt any data.”
