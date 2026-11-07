# ZuzifyOS architecture

**Current target:** experimental Debian 13 Trixie Linux live distribution, KDE Plasma 6 and Wine64. Everything required for the current Linux build is under `distro/`.

The previous project was a freestanding native x86-64 kernel with a separate 32-bit BIOS kernel. It remains on `archive/native-kernel-0.2`; the `kernel/` and `kernel32/` directories in the repo are legacy sources and not used for ZuzifyOS Linux.

Linux provides scheduling, networking, storage, userspace process execution and security features that were absent from the custom kernel. KDE provides a fully functioning configurable desktop and KWin's effects engine. Wine implements a subset of Windows APIs for compatible apps.

The live Linux image is currently a VM-first experiment, **not** a tested installer or guaranteed macOS/Windows equivalent. Updates use Debian APT where persistent; GitHub nightly ISOs are manually replaced.
