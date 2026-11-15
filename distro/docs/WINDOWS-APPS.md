# Windows apps on ZuzifyOS Linux

This release ships Debian's Wine and Wine64 for Windows compatibility.

## Running an EXE

In the ZuzifyOS desktop, **double-click a compatible `.exe` or `.msi` in Files** or open **Start → Run Windows App** to choose a file. The bundled launcher uses Wine and never runs programs automatically just because they were downloaded. You can also use:

```bash
wine /path/to/program.exe
wine msiexec /i /path/to/installer.msi
```

Wine is **not Windows**. Some software needs features Wine has not implemented; games with kernel anti-cheat and programs that install Windows drivers commonly fail. A Windows product key does not change this. Windows Store/UWP software and Microsoft-only system integration are not guaranteed.

Start also includes **Windows App Settings** (`winecfg`) and **Windows App Uninstaller** (`wine uninstaller`) for managing the Wine environment. These tools manage Wine-installed programs only, not native Linux packages.\n\nWine itself is **not a malware sandbox**. Only open Windows programs you trust. Files run with your Linux user's permissions.

32-bit Windows applications may require multilib Wine packages. Debian 13's modern Wine can run some 32-bit software without a 32-bit Linux kernel, but compatibility varies. After installing Debian on a test machine, multiarch can be enabled with:

```bash
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install wine32:i386
```

This may download many packages. It is not bundled into the first live ISO to keep the image smaller and more reliable. Never run random internet EXE installers on your main computer.
