#!/usr/bin/env bash
set -euo pipefail
mkdir -p build
rm -f build/interactive32.log build/interactive32-monitor.log
{
  sleep 3
  echo 'sendkey ret'
  sleep 0.4
  for key in t a i ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 0.3
  for key in 4 3 2 1 ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 0.3
  for key in 4 3 2 1 ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 0.3
  for key in 4 3 2 1 ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 0.4
  for key in h e l p ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 1
  echo quit
} | timeout 45s qemu-system-i386 -m 256M -cpu pentium3 \
  -cdrom build/ZuzifyOS-x86.iso -boot d -display none \
  -serial file:build/interactive32.log -monitor stdio -no-reboot \
  > build/interactive32-monitor.log 2>&1
for marker in ZUZIFYOS32_BOOT_OK ZUZIFYOS32_ACCOUNT_CREATED ZUZIFYOS32_LOGIN_OK ZUZIFYOS32_COMMAND_RECEIVED; do
  if ! grep -Fq "$marker" build/interactive32.log; then
    cat build/interactive32.log build/interactive32-monitor.log
    echo "FAIL: x86 missing marker $marker" >&2
    exit 1
  fi
done
echo "PASS: 32-bit BIOS OOBE, PIN, login, shell input."
