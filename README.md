# Wings crypto learning notebook

Start here when returning to the project. Each lesson is now a separate, runnable Zephyr application. Moving to another lesson does not replace earlier source code.

## What have we done?

We prepared three small demonstrations on the nRF5340 DK:

| Phase | Question it answers | Open the lesson |
| --- | --- | --- |
| 0 | Where should encryption fit in Wings? | [Architecture](lessons/00_architecture/README.md) |
| 1 | Can the board initialize PSA Crypto and generate secure random bytes? | [Random generation](lessons/01_random/README.md) |
| 2 | What is a hash, and how does changing a message affect it? | [SHA-256](lessons/02_sha256/README.md) |
| 3 | Can we encrypt a message and recover it with the same key? | [AES-128-CTR](lessons/03_aes_ctr/README.md) |

All three independent applications have been built with the installed SDK. Successful flashing was reported earlier, and you reported seeing console output. Complete device logs confirming every lesson's result have not been recorded in the conversation. **Build success is not proof of a successful device demonstration.** See [progress and verification](docs/PROGRESS.md).

Phase 3 is educational encryption without authentication. We have not implemented the authenticated EMG-to-PC pipeline yet. Phase 4 is the next crypto lesson, after you are ready; no Phase 4 code was added during this reorganization.

## The easiest way to revisit a lesson

Open a terminal in this `cryptocell` folder:

```sh
./lesson.sh build 1
```

Change `1` to `2` or `3` to select another lesson. The helper sets up the installed SDK and compiler automatically. No repeated `export` commands are needed, including in a new terminal.

To build and program one lesson onto the connected nRF5340 DK:

```sh
./lesson.sh flash 1
```

Flashing changes the program stored on the board. It does not remove any lesson from the Mac. These configurations describe the DK's onboard nRF5340 application core. External-module programming remains a separate, unfinished setup task.

To view output:

```sh
./lesson.sh ports
./lesson.sh monitor /dev/cu.usbmodem0010500464391
```

Use the port actually present on your Mac; the suffix can change. Press RESET after opening the terminal because each lesson prints once per boot. Exit with **Ctrl-A**, then **K**, then **Y**.

Prefer buttons? Read the [VS Code UI instructions](docs/SETUP.md#using-the-nordic-vs-code-ui).

## Folder map

```text
cryptocell/
├── README.md                     start here
├── lesson.sh                     simple build/flash/monitor helper
├── lessons/
│   ├── 00_architecture/README.md  conceptual lesson, no firmware
│   ├── 01_random/
│   ├── 02_sha256/
│   └── 03_aes_ctr/
├── docs/
│   ├── SETUP.md                  tools, folders, UI, configuration
│   ├── PROGRESS.md               what happened and what is verified
│   ├── SUPERVISOR.md             short presentation notes
│   └── TROUBLESHOOTING.md        errors we encountered
├── src/main.c                    original root Phase 3 project, preserved
├── prj.conf                      original root configuration, preserved
├── CMakeLists.txt                original root build file, preserved
└── build/                        old root build, preserved
```

Each numbered firmware lesson contains its own `README.md`, `src/main.c`, `prj.conf`, `CMakeLists.txt`, and generated `build/`. Edit that lesson's source; do not edit generated files.

The old root project remains as a snapshot of where we were. Use the `lessons/` projects for future learning. The Nordic VS Code application list now points to those three folders.

## A short reminder

- **Random bytes:** newly generated unpredictable data; not encrypted data.
- **Hash:** a deterministic fingerprint; not encryption and not authentication by itself.
- **Encryption:** transforms plaintext into ciphertext and can be reversed with the key and required parameters.
- **Authenticated encryption:** also checks for tampering; this is the next lesson, not yet implemented.

For the supervisor, start with [the presentation notes](docs/SUPERVISOR.md). For the exact history and remaining checks, use [the progress record](docs/PROGRESS.md).
