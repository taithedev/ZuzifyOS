#!/usr/bin/env bash
set -euo pipefail
mkdir -p build
rm -f build/interactive.log build/interactive-monitor.log
echo "Testing full 64-bit OOBE, PIN login, PS/2 keyboard and shell..."
{
  sleep 3
  echo 'sendkey ret'
  sleep 0.5
  for key in t a i ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 0.3
  for key in 4 3 2 1 ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 0.3
  for key in 4 3 2 1 ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 0.3
  for key in 4 3 2 1 ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 1.0
  for key in h e l p ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 0.4
  for key in v e r s i o n ret; do printf 'sendkey %s\n' "$key"; sleep 0.17; done
  sleep 1.0
  echo quit
} | timeout 45s qemu-system-x86_64 -m 512M -machine q35 \
  -cdrom build/ZuzifyOS.iso -boot d -display none \
  -serial file:build/interactive.log -monitor stdio -no-reboot \
  > build/interactive-monitor.log 2>&1
for marker in ZUZIFYOS_ACCOUNT_CREATED ZUZIFYOS_LOGIN_OK 'TERM: COMMANDS:' 'TERM: ZuzifyOS 0.2.0-alpha'; do
  if ! grep -Fq "$marker" build/interactive.log; then
    cat build/interactive.log
    cat build/interactive-monitor.log
    echo "FAIL: missing expected marker: $marker" >&2
    exit 1
  fi
done
echo "PASS: graphical OOBE, account creation, login, shell commands."
