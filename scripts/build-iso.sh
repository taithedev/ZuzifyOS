#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
arch="${1:-x64}"
if [[ "$arch" != x64 && "$arch" != x86 ]]; then
  echo "Usage: $0 x64|x86" >&2; exit 2
fi
mkdir -p build
if [[ ! -f limine-binary/limine.c ]]; then
  echo "Downloading official Limine binary release..."
  curl -fL --retry 3 -o build/limine-binary.tar.gz "${LIMINE_BINARY_URL:-https://github.com/Limine-Bootloader/Limine/releases/latest/download/limine-binary.tar.gz}"
  tar -xzf build/limine-binary.tar.gz
fi
make -C limine-binary
rm -rf .iso-work
mkdir -p .iso-work/boot/limine
if [[ "$arch" == x64 ]]; then
  destination="build/ZuzifyOS.iso"
  cp build/kernel.elf .iso-work/boot/
  cp limine.conf .iso-work/boot/limine/limine.conf
else
  destination="build/ZuzifyOS-x86.iso"
  cp build/kernel32.elf .iso-work/boot/
  cp limine32.conf .iso-work/boot/limine/limine.conf
fi
cp limine-binary/limine-bios.sys limine-binary/limine-bios-cd.bin .iso-work/boot/limine/
if [[ "$arch" == x64 ]]; then
  mkdir -p .iso-work/EFI/BOOT
  cp limine-binary/limine-uefi-cd.bin .iso-work/boot/limine/
  cp limine-binary/BOOTX64.EFI .iso-work/EFI/BOOT/
  xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
    -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
    -efi-boot-part --efi-boot-image --protective-msdos-label \
    .iso-work -o "$destination"
else
  # Use the same hybrid El Torito layout so Limine can find a valid partition map.
  # This x86 kernel is verified for BIOS only; the EFI fallback is not a claim
  # of a supported 32-bit UEFI OS environment.
  mkdir -p .iso-work/EFI/BOOT
  cp limine-binary/limine-uefi-cd.bin .iso-work/boot/limine/
  cp limine-binary/BOOTX64.EFI .iso-work/EFI/BOOT/
  xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
    -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
    -efi-boot-part --efi-boot-image --protective-msdos-label \
    .iso-work -o "$destination"
fi
./limine-binary/limine bios-install "$destination"
rm -rf .iso-work
echo "Built $destination ($arch)"
