#!/bin/bash
# Build one independent lesson with the SDK already installed on this Mac.
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

usage() {
    cat <<'HELP'
Usage (from the cryptocell directory):
  ./lesson.sh build 1       Build the random-byte lesson
  ./lesson.sh build 2       Build the SHA-256 lesson
  ./lesson.sh build 3       Build the educational AES-CTR lesson
  ./lesson.sh flash 1       Build and flash the selected lesson (1, 2, or 3)
  ./lesson.sh ports         List macOS serial ports
  ./lesson.sh monitor PORT  Open a serial port at 115200 baud

Each lesson has its own build directory. Flashing replaces the program on
the connected target, but never overwrites another lesson's source files.
These builds target the nRF5340 DK application core, not a custom module.
HELP
}

action="${1:-help}"
case "$action" in
    help|-h|--help) usage; exit 0 ;;
    ports)
        for serial_port in /dev/cu.*; do
            [[ -e "$serial_port" ]] && printf '%s\n' "$serial_port"
        done
        exit 0
        ;;
    monitor)
        serial_port="${2:-}"
        if [[ -z "$serial_port" || ! -e "$serial_port" ]]; then
            echo 'Choose an existing port from: ./lesson.sh ports' >&2
            exit 2
        fi
        echo 'Press the DK RESET button after the terminal opens.'
        echo 'Exit screen with Ctrl-A, then K, then Y.'
        exec screen "$serial_port" 115200
        ;;
    build|flash) ;;
    *) usage >&2; exit 2 ;;
esac

case "${2:-}" in
    1) lesson_name=01_random ;;
    2) lesson_name=02_sha256 ;;
    3) lesson_name=03_aes_ctr ;;
    *) echo 'Choose lesson 1, 2, or 3.' >&2; exit 2 ;;
esac

sdk_dir="${WINGS_NCS_DIR:-/opt/nordic/ncs/v2.4.0}"
toolchain_dir="${WINGS_TOOLCHAIN_DIR:-/opt/nordic/ncs/toolchains/4ef6631da0}"
if [[ ! -f "$sdk_dir/zephyr/zephyr-env.sh" || ! -x "$toolchain_dir/bin/west" ]]; then
    echo 'SDK/toolchain not found. See docs/SETUP.md for the verified paths.' >&2
    exit 2
fi
export PATH="$toolchain_dir/bin:$PATH"
export ZEPHYR_BASE="$sdk_dir/zephyr"
export ZEPHYR_TOOLCHAIN_VARIANT=zephyr
export ZEPHYR_SDK_INSTALL_DIR="$toolchain_dir/opt/zephyr-sdk"
export CCACHE_DIR="$project_dir/.cache/ccache"

board=nrf5340dk_nrf5340_cpuapp
app_dir="$project_dir/lessons/$lesson_name"
build_dir="$app_dir/build"

# -p auto discards generated build files only if the cached board/application
# is incompatible. It does not delete source files or other lesson builds.
if [[ -f "$build_dir/build.ninja" ]] &&
   grep -Fqx "BOARD:STRING=$board" "$build_dir/CMakeCache.txt" &&
   grep -Fqx "APPLICATION_SOURCE_DIR:PATH=$app_dir" "$build_dir/CMakeCache.txt" &&
   grep -Fqx "USER_CACHE_DIR:UNINITIALIZED=$project_dir/.cache/zephyr" "$build_dir/CMakeCache.txt"; then
    # Reuse the verified configuration. Passing new CMake arguments every time
    # would force a reconfiguration even when no source file changed.
    west build -b "$board" -d "$build_dir"
else
    west build -p auto -b "$board" -d "$build_dir" "$app_dir" -- \
        -DNCS_TOOLCHAIN_VERSION=NONE \
        -DUSER_CACHE_DIR="$project_dir/.cache/zephyr"
fi

if [[ "$action" == flash ]]; then
    # The build above must succeed before this command can run (set -e).
    west flash -d "$build_dir" --skip-rebuild --runner nrfjprog
fi
