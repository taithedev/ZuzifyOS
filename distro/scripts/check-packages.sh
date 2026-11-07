#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
missing=0
while IFS= read -r line || [[ -n "$line" ]]; do
  name="${line%%#*}"
  name="$(echo "$name" | xargs)"
  [[ -z "$name" ]] && continue
  if ! apt-cache show "$name" >/dev/null 2>&1; then
    printf 'MISSING Debian package: %s\n' "$name" >&2
    missing=1
  fi
done < config/package-lists/zuzify-desktop.list.chroot
if [[ "$missing" != 0 ]]; then
  echo 'Package preflight failed; check Debian 13 Trixie package naming.' >&2
  exit 1
fi
echo 'PASS: all requested Debian package names resolved.'
