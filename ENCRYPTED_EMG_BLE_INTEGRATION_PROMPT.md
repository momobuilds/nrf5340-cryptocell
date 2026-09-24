# Implementation Prompt: Encrypted EMG over BLE

You are working on the LWT3 project. Implement an end-to-end prototype that encrypts EMG data in the nRF5340 Wings firmware and decrypts it in the RawPower PC application.

## Repositories

The current working directory is the `cryptocell` repository:

- `/Users/mosi_dev/Developer/LWT3/cryptocell`
- `/Users/mosi_dev/Developer/LWT3/wings-fw`
- optional standalone Python prototype folder created in a new feature branch for the receiver and visual demo work

The repositories have different responsibilities:

- `cryptocell`: educational PSA Crypto lessons. Lesson 4 demonstrates AES-128-CCM with simulated samples. Treat it as reference material unless a shared helper is genuinely useful.
- `wings-fw`: nRF5340 embedded firmware. It reads the ADS1298, filters samples, calculates features, and exposes Nordic UART Service (NUS) over BLE.
- standalone Python prototype: a simple receiver app, not the full RawPower desktop application. It should be intentionally lightweight, easy to explain, and designed to visualize accepted/rejected packets for a supervisor demo.

Do not modify the original `cryptocell/src/main.c` snapshot. Do not replace or delete earlier lessons.

## Working branch safety requirement

Create and work on a dedicated feature branch before making any changes, for example:

```bash
git checkout -b feature/encrypted-emg-demo
```

Keep the original repository state intact. Do not commit directly to the main branch. The goal is to protect the original working code while iterating on the secure prototype.

## Goal

Create the smallest working prototype with this behavior:

```text
ADS1298 / Wings firmware
  -> collect one complete EMG or feature payload
  -> serialize a defined application packet
  -> AES-128-CCM encrypt and authenticate it
  -> send the encrypted packet through BLE NUS

Standalone Python receiver demo
  -> connect to the Wings BLE NUS service or load a saved capture
  -> reassemble complete notifications into packets
  -> verify and decrypt AES-128-CCM
  -> reject unauthenticated packets
  -> decode and record valid EMG data
  -> show accepted/rejected packet counters in a simple visual dashboard or plot
```

Begin with the currently active BLE feature path if it is the smallest working path. Clearly distinguish feature packets from raw ADC packets. Do not claim that raw EMG is protected if only `features_t` is protected.

For demonstration purposes, the Python receiver should be intentionally simple and visual: a small script or app that can display packet counters, a live status panel, and optionally a small plot or waveform view for the decoded EMG data so a supervisor can understand what is happening without reading raw protocol details.

## Existing code facts to verify before editing

Inspect the current files; do not rely blindly on this prompt if the code has changed.

In `wings-fw`:

- `src/ADS1298.c` reads eight 24-bit ADC channels.
- `src/main.c` contains `sensor_work_handler()`.
- `sensor_work_handler()` feeds filters/features and sets `status.featuresready`.
- The active BLE path calls `ble_put((uint8_t *)&features[i], sizeof(features_t))` for each feature.
- Raw `frame_t` EMG packets are built under `FEATURE_ENABLE_USB`, but `FEATURE_ENABLE_USB` is currently `0`.
- `src/ble.c` uses Zephyr Nordic UART Service and `ble_put()` calls `bt_nus_send()`.
- The firmware advertises a dynamic RawPower-style BLE name.
- `src/protocol.h` currently defines an old fixed frame format with a simple XOR checksum. That checksum is not cryptographic authentication.

In the standalone Python prototype:

- use a simple Python module or script, not the full RawPower desktop application
- prefer a small BLE receiver built with `bleak` or a saved-capture parser for the first milestone
- keep the receiver independent from the embedded firmware code path and easy to explain in demos
- add a simple visual dashboard or plot using lightweight Python tooling so the supervisor can see packet acceptance, rejection, and decoded signal output
- do not require a full RawPower GUI migration unless it is needed for a demo and kept isolated from the original project

In `cryptocell`:

- Lesson 4 uses PSA AES-128-CCM.
- The demonstration uses a 13-byte nonce and a 16-byte authentication tag.
- PSA AEAD output is `ciphertext || tag`; the nonce and AAD are separate.
- The Lesson 4 key is public and must remain labelled as a test key only.

## Security and protocol requirements

1. Use AES-128-CCM through the existing platform APIs. In the firmware use PSA Crypto and the existing CryptoCell-capable configuration where appropriate. Do not implement AES, CCM, or cryptographic primitives manually.
2. Use a 16-byte tag.
3. Use a 13-byte nonce for this prototype.
4. Never reuse a nonce with the same key. A random nonce alone is acceptable only for this prototype and must be documented as non-production. Add a sequence number and explain the replay limitation.
5. Use a fixed public test key only for the first interoperability test. Label it `TEST KEY - NOT FOR PRODUCTION` in both projects. Do not present it as secure key provisioning.
6. Do not use decrypted output unless the AEAD call succeeds.
7. Authentication must cover the packet metadata through AAD. The PC must reconstruct the exact same AAD bytes.
8. Reject malformed packets, unsupported versions/types, invalid lengths, failed authentication, and replayed or non-increasing sequence numbers when a session policy is available.
9. BLE notification boundaries are not packet boundaries. Implement buffering and packet reassembly in the PC receiver.
10. Do not mix human-readable logs into the binary encrypted packet stream without a defined framing rule.
11. Preserve compatibility with the existing unencrypted path unless a deliberate configuration switch selects encrypted mode.

## Proposed packet format

Use an explicit byte format. Do not encrypt C structs by copying their memory layout.

Choose and document one exact format, preferably:

```text
magic              2 bytes  fixed marker, for example A5 5A
version            1 byte
packet_type        1 byte  feature or raw EMG
sequence           4 bytes unsigned little endian
payload_length     2 bytes unsigned little endian
nonce              13 bytes
ciphertext         payload_length bytes
tag                16 bytes
```

Use these bytes as AAD:

```text
version || packet_type || sequence || payload_length
```

The magic bytes are framing bytes and do not need to be authenticated if the parser rejects them before decryption. Document this decision. Do not add an unauthenticated mutable field after encryption.

If the existing NUS or application protocol requires a different format, adapt it carefully and document the final wire format in both repositories.

## Firmware implementation tasks

1. Add or reuse the PSA Crypto configuration in `wings-fw`.
2. Initialize PSA Crypto once during startup and handle initialization errors.
3. Import the test AES-128 key with CCM encrypt permission.
4. Add a focused crypto helper module rather than putting all crypto logic in `main.c`.
5. Serialize either:
   - the active `features_t` payload, or
   - one defined raw EMG packet.
6. Build the AAD and packet header explicitly with little-endian helpers.
7. Generate a fresh nonce per packet or define a carefully justified session nonce/counter scheme. Never use a fixed zero nonce.
8. Call PSA AEAD encryption and append the 16-byte tag.
9. Send the complete framed encrypted packet through `ble_put()`.
10. Keep the existing BLE command callback path intact.
11. Avoid performing slow cryptography directly inside a high-priority ADC interrupt. Copy data into a work item or queue and encrypt from a suitable worker context if needed.
12. Add logging that reports packet counters and encryption failures without printing the key or plaintext in normal operation.
13. Destroy volatile PSA key objects during shutdown or cleanup paths where practical.

## Standalone Python prototype implementation tasks

1. Create a small packet parser module with no dependency on the RawPower codebase.
2. Add `cryptography` as the Python AES-CCM dependency, or use an already approved equivalent.
3. Implement a pure function similar to:

```python
def decrypt_wings_packet(packet: bytes, key: bytes, last_sequence: int | None):
    """Return decoded metadata and plaintext, or raise a clear validation error."""
```

4. Parse and validate magic, version, type, lengths, nonce, ciphertext, and tag.
5. Reconstruct AAD exactly as the firmware does.
6. Call `AESCCM(key, tag_length=16).decrypt(nonce, ciphertext + tag, aad)`.
7. Catch authentication failures and return a rejected-packet result. Never plot, save, or expose unauthenticated plaintext.
8. Decode the authenticated feature or raw EMG payload using explicit endianness and signedness.
9. Reassemble BLE notifications before parsing. Handle multiple packets in one notification and partial packets across notifications.
10. Add a command-line or small standalone test path before wiring the visual demo.
11. Build a lightweight visual proof-of-concept that shows counters for received, accepted, rejected, malformed, and replayed packets, plus an optional waveform or signal plot for the decoded EMG values.
12. Do not treat encrypted packet bytes as the old `data[1]` heart-rate-like value.
13. Keep BLE connection and packet decryption separate so the same parser can later consume a serial capture or saved log.
14. Keep the visual demo intentionally simple and non-invasive; it should be easy to show to a supervisor without requiring the full RawPower application.

## Tests required

Add focused tests and run them before claiming completion:

1. Firmware builds for the actual Wings/nRF5340 target.
2. Python decrypts a known packet generated by the firmware or a shared test vector.
3. Correct key, nonce, AAD, ciphertext, and tag produce the original payload.
4. One changed ciphertext byte is rejected.
5. One changed AAD/header byte is rejected.
6. One changed tag byte is rejected.
7. Wrong key is rejected.
8. Truncated and oversized packets are rejected without crashing.
9. Partial and combined BLE notifications are reassembled correctly.
10. Replayed sequence numbers are rejected when replay protection is enabled.
11. The old plaintext path remains unchanged when encrypted mode is disabled.

For the first milestone, a saved packet test is sufficient. Then test a live BLE notification from Wings. Do not claim end-to-end success from compilation alone.

## Verification workflow

Use this order:

1. Inspect the current firmware build configuration and board target.
2. Implement the packet format and crypto helper.
3. Build the firmware.
4. Implement the Python decrypt/parser unit tests.
5. Generate or capture one encrypted packet.
6. Run the Python known-packet test.
7. Flash the firmware to the actual nRF5340 hardware.
8. Connect to the advertised BLE NUS service from the PC.
9. Confirm accepted packets and deliberately corrupt one packet to confirm rejection.
10. Record the firmware, BLE, and PC logs.

Report separately:

- firmware compilation result
- flash/programming result
- BLE connection result
- encryption result
- PC authentication/decryption result
- EMG decoding/display result

## Documentation required

Update documentation with:

- final packet format
- exact AAD bytes
- nonce policy and replay policy
- whether raw EMG or features are protected
- BLE service/characteristic details
- Python prototype setup and dependency installation
- expected accepted and rejected packet output
- simple visual demo description for supervisor review
- explicit warning that the test key is not production key management
- remaining work for secure key provisioning and production deployment
- branch strategy: perform all work on a dedicated feature branch and keep the original code untouched

Keep changes focused. Do not refactor unrelated signal-processing, power-management, or GUI code. Do not commit changes unless explicitly requested.
