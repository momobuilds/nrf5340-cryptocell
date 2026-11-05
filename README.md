# nRF5340 CryptoCell — Securing Wearable EMG Data on the Edge

Hardware-accelerated cryptography on the **Nordic nRF5340 DK** (Zephyr / nRF Connect SDK, PSA Crypto API, Arm CryptoCell-312), built as the security groundwork for **Wings**, a wearable EMG system that streams muscle signals over BLE.

The goal: protect sensor samples **on the device, before they leave it**, so a receiver can both decrypt the data and detect tampering.

```text
EMG ADC → buffer → local features → [ AES-CCM: encrypt + authenticate ] → BLE packet → PC
                                      ▲ this repo
```

## What's implemented

Each step is a standalone Zephyr application, so every concept can be built, flashed and tested on its own.

| # | Topic | What the firmware does | Backend |
|---|---|---|---|
| 0 | [Architecture](lessons/00_architecture/README.md) | Where encryption belongs in the EMG → BLE pipeline | — |
| 1 | [Secure RNG](lessons/01_random/README.md) | Initializes PSA Crypto and generates 16 secure random bytes | CC312 TRNG |
| 2 | [SHA-256](lessons/02_sha256/README.md) | Hashes two packets differing by one character; checked against Python `hashlib` | Oberon |
| 3 | [AES-128-CTR](lessons/03_aes_ctr/README.md) | Encrypts / decrypts a packet with a random IV and verifies a byte-exact round trip | Oberon |
| 4 | [AES-128-CCM (AEAD)](lessons/04_aead_ccm/README.md) | Encrypts 8 simulated EMG samples with AAD, 13-byte nonce and 16-byte tag; receiver authenticates before accepting data | CC312 |

## Status

- All four firmware applications build for `nrf5340dk_nrf5340_cpuapp` (NCS 2.4.0).
- Lesson 1 flashed and running on the DK; on-device transcripts for lessons 2–4 are pending and tracked in [docs/PROGRESS.md](docs/PROGRESS.md).
- CC312 hardware backend confirmed for RNG and AES-CCM by inspecting the compiled driver paths.
- **Next:** deliberate-corruption test (tampered tag/ciphertext must be rejected), then integration into the real EMG → BLE stream.

> ⚠️ Keys in this repo are **public test keys** for learning and demos only — not for production use.

## Build & flash

```sh
./lesson.sh build 4          # build lesson 4 (1–4)
./lesson.sh flash 4          # build + flash to the connected nRF5340 DK
./lesson.sh ports            # list serial ports
./lesson.sh monitor <port>   # open serial console, then press RESET
```

Toolchain details, VS Code (nRF Connect) instructions and known issues: [SETUP](docs/SETUP.md) · [TROUBLESHOOTING](docs/TROUBLESHOOTING.md).

## Repository layout

```text
lessons/            one independent Zephyr app per step (src/, prj.conf, CMakeLists.txt)
docs/               setup, progress/verification log, troubleshooting, presentation notes
lesson.sh           build / flash / monitor helper
src/, prj.conf      original single-project version (kept for reference)
```

## Key concepts

- **Random bytes** — unpredictable data used for keys and nonces; not encryption by itself.
- **Hash** — deterministic fingerprint; detects changes but provides no secrecy or authentication alone.
- **Encryption (CTR)** — confidentiality only; an attacker can still flip bits undetected.
- **Authenticated encryption (CCM)** — confidentiality **and** integrity: the receiver rejects any modified packet.

**Stack:** C · Zephyr RTOS · nRF Connect SDK · PSA Crypto · Arm CryptoCell-312 · nRF5340
