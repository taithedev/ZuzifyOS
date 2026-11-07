# Kernel architecture

Limine provides the initial x86-64 environment, a higher-half ELF image, and framebuffer. ZuzifyOS then owns the CPU loop, draws pixels itself, and polls PS/2 ports.

The graphical desktop and terminal are presently **kernel resident**. The `/usr/bin/*.zs` text scripts run through the shell interpreter, not through a process loader. This is deliberate to keep the first milestone inspectable and small.

A fixed-capacity RAM tree backs filesystem commands; it is volatile and has no persistence or permissions. Serial output is used for boot observability. The update command is informational because secure OS updates are not possible before disk, network, and cryptographic foundations.

Boot protocol ABI data in `limine_abi.h` is a small subset of the documented Limine protocol. Binary bootloader artifacts are downloaded from the Limine project by the ISO script. Keeping this boundary narrow helps later protocol updates.
