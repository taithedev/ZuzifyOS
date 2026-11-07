# ZuzifyOS Linux

**ZuzifyOS Linux** is an independent, experimental **Debian 13 (Trixie)** live distribution with **KDE Plasma 6**, an original macOS-inspired dark glass desktop, native Linux apps, and Wine for compatible Windows programs.

> It is **not macOS** and it is **not Windows**. It does not contain Apple's operating system, proprietary Apple assets or Windows APIs. Not all Windows .exe files will run. The first release is for virtual-machine testing; do not install it over Windows.

## Current direction

| Part | Implementation |
| --- | --- |
| Kernel | Debian Linux (64-bit PC) |
| Desktop | KDE Plasma 6 / KWin |
| Look | Zuzify Glass global theme, floating bottom dock, top menu bar, translucent panels, original wallpaper and icon theme |
| Applications | Dolphin, Konsole, Kate, Firefox, Settings, software center |
| Windows programs | Wine (Windows compatibility layer, not a full Windows VM) |
| Updates | Debian security and app updates via APT; new ISOs built by GitHub Actions |
| Onboarding | Branded first-run welcome / theme chooser (not a secure OS installation wizard) |
| Installation | Live ISO only while installer and partition safety are being tested |

## Get the Linux ISO

A successful Linux build is published at [ZuzifyOS Linux nightly](https://github.com/taithedev/ZuzifyOS/releases/tag/linux-nightly).

Choose **ZuzifyOS-Linux-nightly.iso**. The old `ZuzifyOS-x64-nightly.iso` and `ZuzifyOS-x86-nightly.iso` artifacts on the separate legacy release belong to the previous **custom hobby kernel** and **are not Linux**.

If the Linux release does not exist yet, the build is not finished or did not pass. Check [GitHub Actions](https://github.com/taithedev/ZuzifyOS/actions).

## Try it in VirtualBox

1. Download the Linux ISO from the **linux-nightly** release.
2. In VirtualBox, create a guest with **Linux → Debian (64-bit)**.
3. Allocate **4 GB RAM**, **2 CPUs**, and **128 MB video memory** if available.
4. Attach the ISO to its virtual optical drive. No virtual hard disk is needed for a temporary live demonstration.
5. Boot the VM and select the live system. Use the Applications menu, File Manager, Settings or Terminal.

**Important:** With no virtual hard disk and no install, changes in the live session disappear on restart. If VirtualBox says the CPU does not support 64-bit, fix the VM's CPU/architecture/virtualization settings first. Installing a second 32-bit operating-system edition does not fix x64 host configuration.

## Development

Linux distribution definitions, theme and installer plans live in [distro/](distro/README.md). A GitHub Actions job validates the config, then builds an amd64 Debian hybrid ISO on pushes to `main` and when manually dispatched.

Build locally only on a disposable Debian 13 builder VM/container that supports nested mounts and has enough disk space. **Do not run the privileged builder on a daily-use Windows system.**

```sh
python3 distro/scripts/validate.py
# On a throwaway Debian 13 root builder:
cd distro
bash scripts/build.sh
```

## Native kernel preserved

Earlier ZuzifyOS hobby-kernel source was preserved in the branch [archive/native-kernel-0.2](https://github.com/taithedev/ZuzifyOS/tree/archive/native-kernel-0.2). Files under `kernel/` and `kernel32/` are deprecated and are **not** used for the Linux distribution.

## Project status

The Linux ISO workflow must complete successfully before this is called a tested bootable Linux image. This project is not affiliated with Apple, Microsoft, Debian, or KDE; credit and licenses remain with upstream projects. The Zuzify artwork and project-specific files are original contributions.

## Docs

- [Live image build](distro/README.md)
- [Design details](distro/docs/DESIGN.md)
- [Windows compatibility](distro/docs/WINDOWS-APPS.md)
- [Updates](distro/docs/UPDATES.md)
- [Release checklist](distro/docs/RELEASE-CHECKLIST.md)
