# Progress and verification record

Updated: 2026-09-23. This separates what we prepared from what was demonstrated on hardware.

## What happened in our sessions

1. We inspected NCS 2.4.0 and Zephyr `v3.3.99-ncs1` on your Mac and selected `nrf5340dk_nrf5340_cpuapp`.
2. We discussed sample buffering, optional local DSP, packetization, and protection before transport. You correctly placed local feature extraction before encryption.
3. We created Phase 1: initialize PSA and print 16 secure random bytes. You reported seeing console output and asked why randomness is useful. We explained that these printed bytes are test data; future secret keys can use secure generation.
4. We explained the difference between build, flash, running on the board, and reading serial output. We also explained `export`, generated build files, and the harmless `ninja: no work to do` message.
5. Your flash logs showed successful programming/verification alongside repeated J-Link `-256` messages. A compatibility issue between nrfjprog 10.24.2 and J-Link 9.24a is the likely explanation; it has not been repaired here.
6. A later attempt genuinely failed: the build had selected `nrf52840dk_nrf52840`, causing `-f NRF52` against an nRF5340. We rebuilt for the correct nRF5340 application-core target.
7. We created Phase 2: SHA-256 of `EMG test packet` and `EMG test Packet`. A later source inspection found your second message changed to `EMG test Packet!!`. That experiment is documented in the lesson.
8. We created Phase 3: fixed public AES test key, random IV, CTR encryption/decryption, and a byte-for-byte comparison. The application was built; a matching device transcript has not been supplied.
9. You asked about programming an external nRF5340 module through the PCA10095 debugger. The module's exact part number, power voltage, and debug connector are still unknown. No external-target wiring or flashing has been performed by the assistant.
10. We reorganized these lessons into independent projects so returning to one does not overwrite another.

## Where the separated source came from

- **Phase 1:** reconstructed from the earlier Phase 1 code/configuration in the conversation, with the startup banner enabled.
- **Phase 2:** reconstructed from the earlier lesson, using the controlled one-character change as its baseline. The `Packet!!` variation is kept as a documented exercise.
- **Phase 3:** copied from the current root project. In the new copy only, `print_hex()` was corrected to print a newline instead of the literal characters `\n`.
- **Original root project:** `src/main.c`, `prj.conf`, and `CMakeLists.txt` were preserved byte-for-byte, together with the old build and logs. These remain a reference snapshot, while the lesson folders are the maintained learning projects.

These are reconstructed earlier lessons, not claimed recoveries of every historical edit. No Git history was available in this workspace to recover arbitrary intermediate versions.

## Verification status

| Lesson | Build on Mac | Device result recorded |
| --- | --- | --- |
| 1 — random | Passed as an independent application | User saw output; full PSA status/random transcript still needed |
| 2 — SHA-256 | Passed as an independent application | Exact digest comparison on board still needs a recorded transcript |
| 3 — AES-CTR | Passed as an independent application | `Exact match: YES` and successful cleanup still need a recorded transcript |

The compiled backend paths were inspected: RNG calls the Nordic CC312 platform generator; SHA-256 and AES-CTR call Oberon software. This is build evidence. No power measurement, timing benchmark, or direct observation of peripheral activity has been performed.

The SHA-256 reference outputs were independently calculated with Python's standard `hashlib`. The AES test's runtime check exists in firmware; compilation alone does not prove it passed on the board.

## Record your next device run here

After each flash, open the serial monitor and press RESET. Paste the output below or save it as a text file and link it here.

### Phase 1

- Date:
- Board:
- Flash verified:
- `psa_crypto_init: 0`:
- `psa_generate_random: 0`:
- Different random output after reset:
- Output/notes:

### Phase 2

- Date:
- Flash verified:
- Both `psa_hash_compute: 0`:
- Both hashes match the lesson's reference values:
- Same hashes after reset:
- Output/notes:

### Phase 3

- Date:
- Flash verified:
- Encrypt/decrypt statuses are zero:
- `Exact match: YES`:
- `psa_destroy_key: 0`:
- Fresh IV/ciphertext after reset:
- Output/notes:

## Not implemented yet

Authenticated encryption, corruption rejection, experimental nonce/session design, EMG serialization, encrypted transport frames, Python receiver, multi-packet automation, algorithm/power benchmarks, real Wings ADC integration, production key management, authenticated key establishment, separated TF-M services, and secure boot are future phases.

The DK target may enable TrustZone-related hardware configuration by default. That does not mean these lessons implement an isolated secure service or verified secure boot.

Resume with whichever lesson you want to review. Phase 4 should only begin when you explicitly choose to continue.
