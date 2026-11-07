# ZuzifyOS development roadmap

## Alpha 0.1 — bootable foundation
- [x] Freestanding x86-64 kernel with Limine
- [x] Direct framebuffer desktop and keyboard-driven terminal (implementation; CI smoke boot required)
- [x] Writable RAM-only virtual filesystem
- [x] Built-in commands, text scripts, and launcher shortcuts
- [x] Version-tagged ISO workflow and release manifest
- [ ] First verified QEMU boot and display inspection

## Alpha 0.2 — real OS infrastructure
- [ ] Physical memory allocator and virtual memory manager
- [ ] IDT, interrupt handlers, timer and scheduling
- [ ] Userspace ELF64 loader and isolated processes
- [ ] VFS mount API and persistent FAT/ext2 driver
- [ ] Mouse and USB HID drivers

## Alpha 0.3 — applications and graphics
- [ ] Backbuffer/compositor with dirty-region rendering
- [ ] App windows, drag/drop, mouse, window focus
- [ ] Monospace and icon asset pipeline (prefer vector-to-bitmap at build)
- [ ] Terminal ANSI support and pipes
- [ ] ELF64 native applications and basic syscalls

## Alpha 0.4 — network and updates
- [ ] NIC driver and network stack
- [ ] TLS and key store
- [ ] Signed update metadata, download, staging, rollback
- [ ] A/B partitions and recovery environment

## Later
- [ ] POSIX compatibility layer
- [ ] Compatibility research for Windows PE32+ applications
- [ ] Filesystem safety, power management, accessibility
- [ ] Hardware validation

**Reality check:** a compatibility layer for arbitrary Windows .exe programs is a very large separate project. It is not included in the alpha.
