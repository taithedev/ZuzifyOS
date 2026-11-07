# How to try ZuzifyOS on a Windows 11 PC

No previous OS development knowledge is needed to try the current ISO inside a **virtual machine**. A VM lets you boot an experimental OS without overwriting Windows.

## Simple virtual-machine method

1. Open the [GitHub ZuzifyOS nightly release](https://github.com/taithedev/ZuzifyOS/releases/tag/nightly).
2. Download **ZuzifyOS-x64-nightly.iso** for a 64-bit PC. Use **ZuzifyOS-x86-nightly.iso** only if testing the 32-bit BIOS edition.
3. Install a virtual machine application, such as [VirtualBox](https://www.virtualbox.org/).
4. Create a new VM. Choose **Other / Unknown (64-bit)** for x64, or a 32-bit Other/Unknown profile for the x86 edition. If your application uses different names, choose an x86-64 or x86 guest respectively.
5. Give the VM **at least 512 MB RAM** for x64 or **256 MB** for x86.
6. **Do not attach your real Windows drive.** A virtual hard disk is not required for this live alpha, and it currently would not persist your user information anyway.
7. Attach the downloaded ISO as the VM's *virtual optical disc*.
8. Start the VM. You should see a ZuzifyOS welcome screen in the x64 edition, or the text-based welcome prompt on x86.
9. Press Enter, choose a username and PIN, confirm it, and unlock the session.

The x64 edition supports both BIOS and UEFI in QEMU; a virtual machine using EFI can try EFI mode. The x86 edition is **BIOS only**.

## Keys

- Enter: continue in setup, submit PIN, run terminal commands
- Backspace: correct typing
- F1: Terminal
- F2: File Explorer
- F3: Settings
- F4: Applications
- Esc: close the active desktop panel
- In Settings: 1/2/3 select Midnight/Violet/Ocean; L locks the session

The PS/2 mouse is experimental and may only work on virtual machines that emulate a PS/2 pointing device.

## If you see '64-bit is not supported'

That error does not automatically mean the ZuzifyOS kernel is wrong. Check which application is displaying it:

- **VM app won't offer a 64-bit guest:** your host may need hardware virtualization enabled or a VM setting changed. You may ask someone experienced with your PC's firmware settings to help.
- **Limine says unsupported CPU / architecture:** verify that you selected the x64 ISO for an x86-64 VM (not a 32-bit emulated CPU).
- **Black screen on physical hardware:** ZuzifyOS lacks many hardware drivers and is not ready for native installation. Return to a VM instead.
- **UEFI won't boot:** confirm the VM is using x64 UEFI firmware for x64, or switch the VM to legacy BIOS. The x86 ISO is legacy BIOS only.
- **Bootloader loads but keyboard is dead:** ZuzifyOS currently only implements polling PS/2 input, not USB HID/xHCI.

If something fails, provide the exact error text or a screenshot and your VM name. There is no safe general fix without that information.

## Important

- Accounts, PINs, and files **reset after every reboot**.
- This is not a secure login. **Do not reuse an actual device/account PIN.**
- Updates are published as new ISO files. There is **no in-OS automatic updating** yet.
- Never use a disk image writing tool to overwrite your PC's internal disk with this alpha.
