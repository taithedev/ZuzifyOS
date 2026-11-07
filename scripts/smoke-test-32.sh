#!/usr/bin/env bash
set -euo pipefail
mkdir -p build
set +e
timeout 18s qemu-system-i386 -cpu pentium3 -m 256M \
  -cdrom build/ZuzifyOS-x86.iso -boot d -display none \
  -serial stdio -no-reboot > build/boot32.log 2>&1
status=$?
set -e
if [[ "$status" -ne 0 && "$status" -ne 124 ]]; then
  cat build/boot32.log
  exit "$status"
fi
if ! grep -q ZUZIFYOS32_BOOT_OK build/boot32.log; then
  cat build/boot32.log
  echo "FAIL: x86 BIOS boot marker missing" >&2
  exit 1
fi
echo "PASS: x86 32-bit BIOS kernel booted on Pentium III emulation"
