# ZuzifyOS Linux live-image sources

Debian **13 Trixie (amd64)** / KDE Plasma 6. This directory contains all source-controlled configuration for building the **Linux** ISO.

## Why a Linux base

ZuzifyOS now has the real Linux kernel, memory/process isolation, hardware drivers, networking, real packages, native applications and standard login capabilities inherited from Debian. Earlier custom hobby kernels are preserved on `archive/native-kernel-0.2` but are no longer the boot target.

## Build layout

- `auto/config`: reproducible Debian `live-build` options
- `config/package-lists/*.list.chroot`: desktop/kernel/Wine packages
- `config/includes.chroot`: branded Plasma global theme, config, autostart files, system tools
- `assets/wallpaper.svg`: original wallpaper (rasterized to 1920x1080 PNG during build)
- `scripts/build.sh`: root-only live builder
- `scripts/validate.py`: non-root static checks
- `docs`: system documentation

## Recommended build environment

**A disposable Debian 13 amd64 VM or privileged ephemeral CI container**, with approximately **30 GB free disk space**, 8 GB RAM recommended, Internet access and permission to create loop mounts.

On Debian 13:

```sh
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
  live-build debootstrap ca-certificates curl gnupg \
  squashfs-tools xorriso mtools dosfstools \
  syslinux-utils grub-pc-bin grub-efi-amd64-bin \
  grub-common librsvg2-bin python3 git make rsync wget
python3 distro/scripts/validate.py
cd distro
sudo bash scripts/build.sh
```

Output on success:

```
distro/build/ZuzifyOS-Linux-amd64.iso
distro/build/SHA256SUMS.txt
```

This build is **not** a Linux installer, and it does not write to your computer's boot partitions. `live-build` uses mount/chroot operations on its build filesystem, so use a throwaway builder instead of the computer you rely on.

## Architecture

64-bit `amd64` Linux supports many 64-bit and some 32-bit **Windows applications** through Wine. It is not necessary to build a separate 32-bit Linux distribution for that. Wine64 is installed; some 32-bit Windows programs may require optional multiarch packages after installation. The live ISO uses Debian packages and ordinary APT security updates.

KDE Plasma renders the desktop, and KWin supplies compositor effects. Zuzify's original theme configures a top panel, dock, colors, wallpaper, effects and desktop applications. KDE remains user-configurable and is not modified at source.

## CI behavior

Pull requests run **source/static checks** only. Main-branch pushes execute the larger Debian image build, store the ISO as an Actions artifact, and publish it under the dedicated `linux-nightly` GitHub prerelease after success. Git tags beginning with `linux-v` publish versioned experimental releases.

The build artifacts are **not** an in-place signed updater for existing installed systems. Standard Debian security upgrades use APT; release-ISO replacement is still manual.

## Hardware warning

Boot in VirtualBox or QEMU first. Do not flash to your internal Windows disk. macOS-inspired aesthetics are original; this is **not** a macOS clone, Apple-authored desktop, or Windows replacement with guaranteed binary compatibility.
