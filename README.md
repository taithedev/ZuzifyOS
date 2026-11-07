# ZuzifyOS

A **real, experimental operating system** with its own freestanding C kernel. **It is not Linux, Windows, Electron, or a webpage.** ZuzifyOS boots from an ISO image using Limine.

[Boot and test instructions](docs/BOOT-GUIDE.md) | [Roadmap](docs/ROADMAP.md) | [Security limitations](docs/SECURITY-NOTES.md) | [Latest test runs](https://github.com/taithedev/ZuzifyOS/actions)

## Editions

| Image | CPU | Firmware | Interface |
|---|---|---|---|
| `ZuzifyOS-x64-nightly.iso` | x86-64 (64-bit) | Legacy BIOS **and** UEFI | Rounded graphical desktop, translucent glass panels, first-boot wizard, PIN sign-in, settings, terminal, RAM filesystem |
| `ZuzifyOS-x86-nightly.iso` | i686 (32-bit) | **Legacy BIOS only** | Text console, first-boot wizard, PIN sign-in, basic terminal |

**Recommended for most Windows 10/11 computers: x64.** A computer running 64-bit Windows is not guaranteed to boot this experimental OS on real hardware. Testing has only been completed in QEMU.

**Do not install or flash this onto your Windows drive.** Use an isolated virtual machine. This ISO is bootable, but not a full OS installer.

## First boot

64-bit edition:
1. Boot the x64 ISO in QEMU or another virtual machine.
2. At the ZuzifyOS welcome screen, press **Enter**.
3. Create a username with 2–19 letters/numbers/underscores/hyphens.
4. Enter a 4–8 digit PIN and confirm it.
5. Unlock using the same PIN.
6. Use **F1 Terminal**, **F2 Files**, **F3 Settings**, **F4 Applications**. **Esc** closes the current panel.

32-bit BIOS edition: text-based screens provide the same basic name / PIN sequence and a limited shell.

**The current account, PIN and files are kept only in RAM and disappear after a reboot.** They are a UI prototype and do not provide secure access control.

## What works in x64 alpha 0.2

- Freestanding C kernel booted through Limine using either BIOS or UEFI.
- Custom direct framebuffer renderer: rounded panels, lightweight sampled blur/tints, pointer-responsive redraw, taskbar and icon glyphs.
- PS/2 polling keyboard and experimental PS/2 mouse support, mainly for QEMU emulation.
- Account onboarding, PIN creation, login and lock/unlock interface (temporary only).
- Settings: Midnight, Violet, Ocean themes and informational system/account screens.
- File explorer, RAM-only filesystem, text editing and script commands in the terminal.
- GitHub Actions: x64 BIOS / UEFI and x86 BIOS QEMU smoke tests, setup+PIN keyboard tests, nightly ISO releases.

### Terminal examples (x64)

```
help
uname
whoami
ls /
cd /home/guest
mkdir projects
write projects/test.txt hello
cat projects/test.txt
tree /
run /usr/bin/tour.zs
settings
theme ocean
lock
```

## What does **not** work yet

- Permanent account storage or encrypted PIN database
- Proper userspace access control or strong cryptography
- Physical hardware USB/Wi-Fi/GPU drivers and full mouse compatibility
- Real installation, disk partitioning, persistent files, or A/B update system
- True multitasking, separate user processes, native ELF applications
- Running arbitrary Windows `.exe` files
- Actual in-OS network auto-updates

A bootable 32-bit edition does **not** mean the 32-bit edition has every graphical feature of x64. The 32-bit text edition is a compatibility milestone.

## Build and run (Ubuntu or WSL2)

```bash
sudo apt-get update
sudo apt-get install -y build-essential clang lld make curl xorriso qemu-system-x86 ovmf
git clone https://github.com/taithedev/ZuzifyOS.git
cd ZuzifyOS
make all
make run         # 64-bit graphical BIOS VM
make run32       # 32-bit text BIOS VM
make test
make test32
make test-interactive
```

Output: `build/ZuzifyOS.iso` (x64) and `build/ZuzifyOS-x86.iso` (x86).

The build downloads the official Limine binary release. QEMU tests check actual boot markers and inject keyboard input for automated first-run and login verification.

## GitHub releases

Successful main branch builds publish both images under the [nightly prerelease](https://github.com/taithedev/ZuzifyOS/releases/tag/nightly), along with SHA-256 checksums. Git tags like `v0.2.0-alpha` publish versioned assets. **Automatic installation of OS updates is not implemented.**

## License

MIT. This project uses original pixel/vector-style glyphs. It is a hobby project; it is neither endorsed by nor affiliated with Apple or Microsoft.
