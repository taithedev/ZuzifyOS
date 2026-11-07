#!/usr/bin/env bash
set -euo pipefail
mkdir -p build
echo "Booting ZuzifyOS in QEMU (headless, serial log)..."
set +e
timeout 18s qemu-system-x86_64 -m 512M -machine q35 -cdrom build/ZuzifyOS.iso -boot d -display none -serial stdio -no-reboot > build/boot.log 2>&1
status=$?
set -e
if [[ "$status" -ne 0 && "$status" -ne 124 ]]; then
  cat build/boot.log
  echo "QEMU exited unexpectedly: $status" >&2
  exit "$status"
fi
if ! grep -q "ZUZIFYOS_BOOT_OK" build/boot.log; then
  cat build/boot.log
  echo "Boot marker missing: ISO did not reach the kernel entry successfully" >&2
  exit 1
fi
echo "PASS: ZUZIFYOS_BOOT_OK detected in serial log."
