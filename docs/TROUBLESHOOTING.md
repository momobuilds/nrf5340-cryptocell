# Troubleshooting the issues we encountered

[Back to the notebook](../README.md)

## `ninja: no work to do`

This is normal. The selected build is already up to date. It says nothing about which firmware is on the board. Save a source change before rebuilding; flash only when you need to update the board.

## `west: command not found`, compiler missing, or cache permissions

Use `./lesson.sh build 1` (or 2/3) from the root folder. It selects the existing toolchain and writable project-local caches without requiring manual exports. It does not install software.

We previously hit an unwritable cache under the user's Library directory and another under the SDK. Keeping caches in `.cache/` fixes that for the helper. No change to crypto code is needed.

## Source directory differs only by `LWT3` versus `lwt3`

Different tools previously cached different casing for the same macOS path. The helper uses explicit paths and west's `-p auto`, which recreates incompatible generated build files. Do not copy an old build directory between lessons; compile each lesson in its own folder.

## `-f NRF52`, unexpected debug-port ID, or family mismatch

This was a real flash failure. The build targeted `nrf52840dk_nrf52840`, but the connected chip was nRF5340.

For the onboard DK, select `nrf5340dk_nrf5340_cpuapp` and rebuild. The helper selects it explicitly. Confirm in `lessons/<name>/build/CMakeCache.txt`:

```text
BOARD:STRING=nrf5340dk_nrf5340_cpuapp
```

Do not force the flashing tool's family to NRF53 while keeping firmware compiled for nRF52. The firmware itself must be rebuilt for the correct board.

For an external module, the target's actual board configuration must describe its hardware. The debugger's own model is not the firmware board target.

## Repeated `JLinkARM.dll reported error -256`

We observed these with nrfjprog 10.24.2 and J-Link 9.24a, including during a version-only query. A tool-version compatibility problem is the likely explanation; the exact internal cause was not established locally. Nordic support has associated this pattern with nrfjprog/J-Link versions: [support discussion](https://devzone.nordicsemi.com/f/nordic-q-a/127048/writing-into-uicr-otp-of-app-core-with-nrfprog-fails-on-nrf5340).

The line numbers belong to the host programming tool, not `src/main.c`.

Some supplied logs also contained `Write successful`, `Verify successful`, and a final successful-flash message. Those attempts reported success despite the diagnostics. A later family-mismatch log ended in a fatal error and did not succeed. Read the full outcome, not just one line.

If programming and verification succeed, check the firmware's output after RESET. If programming, verification, or startup fails, stop and diagnose it. Changing crypto code will not repair these host-tool diagnostics. No tool upgrade/downgrade, recovery, or mass erase was performed during the lesson reorganization.

## No serial port, disappearing ports, or blank screen

1. Connect the powered DK through the debugger's USB connector with a data-capable cable.
2. Run `./lesson.sh ports` and find the currently visible `/dev/cu.usbmodem...` ports.
3. Open one with `./lesson.sh monitor /dev/cu.usbmodem...`.
4. Press RESET. All lessons print once at boot.
5. If blank, close the monitor (Ctrl-A, K, Y), try the other DK port, and reset again. Close other programs using the same serial port.

The two ports we observed ended in `4391` and `4393`, but their names can change. Missing USB devices require checking the connection, power, cable, or hub; a baud-rate change cannot make a missing device appear.

## I see an older phase's banner

Check the lesson selected in the UI or helper command. Building a new lesson does not flash it automatically unless you use `./lesson.sh flash N`. Verify flashing completed, open the console, and reset.

## External nRF5340 module programming

This setup is still pending. We know only that the module uses nRF5340. We need its exact part number, board schematic/debug pinout, operating voltage/power source, and DK revision before giving definitive wiring.

The PCA10095 provides debug output through P19/P20, but target detection and power connections differ. See [Nordic's external-target guide](https://docs.nordicsemi.com/r/bundle/ug_nrf5340_dk/page/ug/dk/hw_debug_out_segger53.html). Do not assume SWD wiring also routes the module's UART logs to the DK's serial port.
