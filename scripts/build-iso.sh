#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build
if [[ ! -f limine-binary/limine.c ]]; then
  echo "Downloading official Limine binary release..."
  curl -fL --retry 3 -o build/limine-binary.tar.gz "${LIMINE_BINARY_URL:-https://github.com/Limine-Bootloader/Limine/releases/latest/download/limine-binary.tar.gz}"
  tar -xzf build/limine-binary.tar.gz
fi
make -C limine-binary
rm -rf .iso-work
mkdir -p .iso-work/boot/limine .iso-work/EFI/BOOT
cp build/kernel.elf .iso-work/boot/
cp limine.conf limine-binary/limine-bios.sys limine-binary/limine-bios-cd.bin limine-binary/limine-uefi-cd.bin .iso-work/boot/limine/
cp limine-binary/BOOTX64.EFI .iso-work/EFI/BOOT/
xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
  -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
  -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
  -efi-boot-part --efi-boot-image --protective-msdos-label \
  .iso-work -o build/ZuzifyOS.iso
./limine-binary/limine bios-install build/ZuzifyOS.iso
rm -rf .iso-work
echo "Built build/ZuzifyOS.iso"
