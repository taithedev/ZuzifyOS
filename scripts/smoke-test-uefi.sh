#!/usr/bin/env bash
set -euo pipefail
firmware=""
for path in /usr/share/OVMF/OVMF_CODE_4M.fd /usr/share/OVMF/OVMF_CODE.fd; do
  if [[ -f "$path" ]]; then firmware="$path";break;fi
done
if [[ -z "$firmware" ]]; then
  echo "FAIL: OVMF firmware not installed; install package ovmf" >&2
  exit 1
fi
set +e
timeout 22s qemu-system-x86_64 -m 512M -machine q35 \
  -drive if=pflash,format=raw,readonly=on,file="$firmware" \
  -cdrom build/ZuzifyOS.iso -display none -serial stdio -no-reboot \
  > build/uefi.log 2>&1
status=$?
set -e
if [[ "$status" -ne 0 && "$status" -ne 124 ]]; then
  cat build/uefi.log
  echo "UEFI QEMU exited with status $status" >&2
  exit "$status"
fi
if ! grep -q ZUZIFYOS_BOOT_OK build/uefi.log; then
  cat build/uefi.log
  echo "FAIL: UEFI boot did not reach ZuzifyOS kernel" >&2
  exit 1
fi
echo "PASS: 64-bit UEFI boot under QEMU OVMF"
