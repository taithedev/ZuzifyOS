# ZuzifyOS

**ZuzifyOS** is an experimental, real, freestanding x86-64 hobby operating system. It boots with the [Limine](https://github.com/limine-bootloader/limine) bootloader; it is **not** Windows, Linux, Electron, or a web simulation.

## Current scope (v0.1.0-alpha)

- Bootable BIOS/UEFI hybrid ISO and an x86-64 ELF kernel.
- Direct framebuffer rendering with a dark desktop, vector-style icons, taskbar, and terminal.
- Polling PS/2 keyboard driver (QEMU / legacy keyboard emulation).
- A command interpreter, writable **RAM-only** virtual filesystem, and built-in apps / shell scripts.
- Serial boot diagnostic output for automated QEMU smoke tests.
- GitHub Actions ISO builds, version-tagged releases, checksums, and an informational update manifest.

**Not yet included:** real disk storage, multitasking, userspace processes, mouse support, Wi-Fi or Ethernet, a network updater, secure boot integration, signed updates, ELF app loading, or Windows .exe compatibility. All filesystem changes disappear on reboot. This is an early-stage OS project, not something to install over Windows.

## Build

On Ubuntu/Debian or WSL2:

```bash
sudo apt-get update
sudo apt-get install -y build-essential clang lld make curl xorriso qemu-system-x86
git clone https://github.com/taithedev/ZuzifyOS.git
cd ZuzifyOS
make iso
make run
```

The build downloads the official Limine binary release during ISO creation. A network connection is needed on the first build. Output: `build/ZuzifyOS.iso`.

**Windows users:** WSL2 (Ubuntu) is an easy build environment; run QEMU outside or inside WSL depending on your display setup. Start with a virtual machine rather than real hardware.

## Terminal hotkeys

ZuzifyOS polls for PS/2 keyboard input. Type `help` for available commands; `F1` shows the terminal, `F2` lists files, `F3` shows system information, and `F4` lists apps. A physical USB keyboard is **not** guaranteed to work; QEMU emulates PS/2.

Try:

```
help
ls /
tree /
cat /home/guest/welcome.txt
mkdir /home/guest/projects
write /home/guest/projects/notes.txt hello from zuzifyos
cat /home/guest/projects/notes.txt
run /usr/bin/hello.zs
apps
update check
```

`run` executes built-in app commands and simple `.zs` text scripts, **not native x86 executable files**.

## CI and releases

Pushes and pull requests build the ISO and execute an automated QEMU boot-log smoke test. A tag such as `v0.1.0-alpha` also publishes a GitHub Release with an ISO, SHA-256 checksum, and JSON metadata. These are **release checks**, not in-OS automatic updates: a secure updater requires networking, persistent storage, signatures, and rollback.

## Repository layout

- `kernel/include` — freestanding kernel interfaces and Limine ABI types
- `kernel/src/boot` — boot handoff and diagnostics
- `kernel/src/drivers` — keyboard and serial drivers
- `kernel/src/graphics` — framebuffer, font, widgets, icons
- `kernel/src/fs` — RAM filesystem
- `kernel/src/shell` — terminal and commands
- `kernel/src/lib` — freestanding runtime helpers
- `scripts/` — ISO tooling, CI smoke checks
- `.github/workflows/` — builds and releases

## Safety and architecture

Boot only in QEMU until device support and storage safety have been independently reviewed. Never overwrite a disk with an experimental OS image. This project has not been verified on physical hardware.

## License

MIT; see LICENSE.
