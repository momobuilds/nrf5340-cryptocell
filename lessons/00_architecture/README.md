# Phase 0 — Where cryptography fits in Wings

[Back to the notebook](../../README.md)

## What we are learning

Decide where data protection belongs without committing to the final Wings design.

```text
Simulated samples now / real ADC samples later
                    ↓
                Sample buffer
                    ↓
        Optional local signal processing
                    ↓
        Serialize an application packet
                    ↓
          Authenticated encryption
                    ↓
              USB / BLE / other
                    ↓
       Receiver authenticates and decrypts
                    ↓
          Parse / process / store
```

This is the intended future path, not a statement that all these blocks are implemented.

Serialization means writing fields into a defined byte sequence. The crypto function receives bytes, so changing the source from a simulated array to ADC acquisition should not require changing the crypto interface.

## Three possible approaches

| Approach | Benefit | Cost or limitation |
| --- | --- | --- |
| A: protect each sample separately | Very small acquisition delay | Repeats security metadata and API work for each tiny sample |
| B: collect samples, packetize, protect the packet | Preserves raw data for PC processing; shares overhead across samples | Buffering adds latency and raw samples require bandwidth |
| C: collect samples, calculate features, packetize, protect | May reduce the amount of transmitted data | Requires local computation and can discard raw information |

B and C are usually more useful starting points for an EMG wearable. The correct choice depends on latency, battery, bandwidth, and whether the PC needs the raw waveform. Wings could eventually use both packet types. We have not selected a final architecture.

The proposed eight `uint16_t` simulated samples occupy 16 bytes before headers and security metadata. That array has not yet been integrated into the current lessons.

## Two decisions you already understood

1. Replacing simulated data with ADC data primarily changes the producer that fills the sample buffer. The packet and crypto interfaces can stay the same.
2. Feature extraction on the MCU happens before encryption because it needs readable samples.

## What threat are we addressing?

Encryption is intended to prevent an observer without the key from reading transmitted data. Authentication is intended to detect modification or forgery. These are separate properties; Phase 3 demonstrates only the first mechanism using a public test key.

## Files, build, and expected result

This phase is documentation only. No code, build, flash, or device output is required.

Try explaining why the MCU must filter its EMG data before encrypting it, and why one packet of eight samples can be more efficient than eight individually protected samples.

**Stop after the concept is clear.** The next independent project is [Phase 1](../01_random/README.md).
