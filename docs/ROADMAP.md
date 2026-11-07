# ZuzifyOS roadmap

## 0.2-alpha - current
- [x] x64 higher-half kernel boot under Limine on QEMU BIOS and UEFI
- [x] x86 32-bit independent BIOS Multiboot2 kernel under QEMU
- [x] x64 dark graphical desktop with rounded corners and sampled translucent panels
- [x] x64 first-boot wizard, local session username/PIN setup and lock screen
- [x] x64 keyboard navigation and simulated PS/2 mouse support
- [x] x64 Settings page with three themes
- [x] x64 RAM-only file browser, terminal, commands and text scripts
- [x] x86 text-mode setup, PIN unlock and basic shell
- [x] Continuous integration checking both architecture boots and setup input
- [ ] Real-world screenshot and broad UI/usability review
- [ ] Real device / multiple virtual machine hardware validation

## 0.3-alpha - kernel fundamentals
- [ ] Physical page allocator, proper virtual memory management
- [ ] IDT, interrupts, timers, task/process scheduler
- [ ] User mode isolation, syscalls and ELF64 application loader
- [ ] Robust USB HID and mouse input support
- [ ] Graphics double buffering, proper compositor and windows
- [ ] Persistent virtual filesystem and safe disk driver
- [ ] Persistent account DB with salted password hashing and secure handling
- [ ] x86 GUI feature parity

## 0.4-alpha - networking and security
- [ ] Ethernet NIC driver, network stack and time sync
- [ ] Cryptographic primitives, key store, verified update manifests
- [ ] Secure update download/staging/rollback and A/B partitions
- [ ] Robust filesystem permissions and sandboxing

## Later
- [ ] Native app ecosystem and package manager
- [ ] Power management, accessibility and internationalization
- [ ] Windows .exe compatibility research (PE loader, Win32 APIs or equivalent)
- [ ] Installation and recovery tooling, extensive hardware tests

**Neither x64 nor x86 image is presently suitable as a full replacement for Windows.**
