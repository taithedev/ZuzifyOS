# ZuzifyOS Linux

**ZuzifyOS Linux** is an independent, experimental **Debian 13 (Trixie)** live distribution with **KDE Plasma 6**, an original Windows-inspired dark glass desktop, native Linux apps, and Wine for compatible Windows programs.

> It is **not macOS** and it is **not Windows**. It does not contain Apple's operating system, proprietary Apple assets or Windows APIs. Not all Windows .exe files will run. The first release is for virtual-machine testing; do not install it over Windows.

## Current direction

| Part | Implementation |
| --- | --- |
| Kernel | Debian Linux (64-bit PC) |
| Desktop | KDE Plasma 6 / KWin |
| Look | Zuzify Glass global theme, single bottom taskbar, Start menu, translucent panels, original wallpaper and icon theme |
| Applications | Dolphin, Konsole, Kate, Firefox, Settings, software center |
| Windows programs | Wine, plus file-manager double-click associations and app settings (not full Windows compatibility) |
| Updates | Debian security and app updates via APT; new ISOs built by GitHub Actions |
| Onboarding | Branded first-run welcome / theme chooser (not a secure OS installation wizard) |
| Installation | Live ISO only while installer and partition safety are being tested |

## Download ZuzifyOS Windows Preview

The official experimental Linux preview is published at [ZuzifyOS Windows Preview](https://github.com/taithedev/ZuzifyOS/releases/tag/windows-preview) **only after the GitHub Actions ISO build succeeds**.

GitHub Releases limits each uploaded asset to 2 GiB, while the complete ZuzifyOS ISO is larger. To obtain a usable ISO on Windows:

1. Download **all** `ZuzifyOS-Windows-Preview.iso.partNNN` files from the release.
2. Download `ZuzifyOS-Windows-Preview.iso.sha256` and `Rebuild-ZuzifyOS-ISO.ps1` to the **same folder**.
3. Open PowerShell in that folder and run:

```powershell
powershell -NoProfile -File .\Rebuild-ZuzifyOS-ISO.ps1
```

4. The script rebuilds `ZuzifyOS-Windows-Preview.iso` and verifies its SHA256 hash before reporting success.
5. Attach the reconstructed ISO to your VM's optical drive and boot it.

The script does not write to any disk partition. If execution policies prevent the script from running, inspect its contents and use the documented Windows PowerShell execution policy options; do not disable system protections permanently.

The old `ZuzifyOS-x64-nightly.iso` and `ZuzifyOS-x86-nightly.iso` assets on the separate [legacy release](https://github.com/taithedev/ZuzifyOS/releases/tag/nightly) belong to the previous hobby kernel. They do not contain the Debian 13 Windows-style desktop.

## Try it in VirtualBox

1. Download and reconstruct the ISO from the **windows-preview** release using the instructions above.
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
