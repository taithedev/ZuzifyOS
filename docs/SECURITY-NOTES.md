# Security notes: ZuzifyOS 0.2 alpha

**The PIN interface is not secure authentication.**

- The username and short PIN verifier live in kernel memory and are deleted when the VM reboots.
- The current PIN verifier uses a simple, non-cryptographic in-memory checksum, not a secure password hashing scheme.
- There is no persistent account database, entropy source, secure boot verification, user-mode boundary or filesystem permissions.
- The screen lock is a UI demonstration, not protection against an attacker with access to the kernel or machine.
- Never reuse a real password or PIN with this prototype.
- ZuzifyOS is intended for offline VM testing; do not expose it to untrusted networks.
- No automatic OS updates occur. GitHub nightly releases are downloadable images, not authenticated in-OS patch installations.
- The included 32-bit kernel is BIOS-only; its VGA screen assumes a conventional legacy environment.

Before genuine secure accounts can ship, ZuzifyOS will need persistent storage, a cryptographically sound password/PIN KDF with per-account salts, proper memory isolation, account lockout policies, a measured/signed boot chain, recovery options and extensive testing.
