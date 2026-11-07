#!/usr/bin/env bash
set -euo pipefail
mkdir -p build
rm -f build/interactive.log build/interactive-monitor.log
echo "Testing PS/2 keyboard input, 'help', and 'version' through QEMU..."
{
  sleep 4
  for key in h e l p ret; do
    printf 'sendkey %s\n' "$key"
    sleep 0.18
  done
  sleep 0.6
  for key in v e r s i o n ret; do
    printf 'sendkey %s\n' "$key"
    sleep 0.18
  done
  sleep 1
  echo quit
} | timeout 35s qemu-system-x86_64 -m 512M -machine q35 \
    -cdrom build/ZuzifyOS.iso -boot d -display none \
    -serial file:build/interactive.log -monitor stdio -no-reboot > build/interactive-monitor.log 2>&1
if ! grep -q 'TERM: COMMANDS:' build/interactive.log; then
  cat build/interactive.log
  cat build/interactive-monitor.log
  echo "FAIL: 'help' was not processed by keyboard + shell" >&2
  exit 1
fi
if ! grep -q 'TERM: ZuzifyOS 0.1.0-alpha' build/interactive.log; then
  cat build/interactive.log
  cat build/interactive-monitor.log
  echo "FAIL: 'version' was not processed by keyboard + shell" >&2
  exit 1
fi
echo "PASS: help and version executed using emulated PS/2 keystrokes."
