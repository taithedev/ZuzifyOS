# ZuzifyOS Linux updates

## Security & application packages

ZuzifyOS uses Debian APT repositories. `unattended-upgrades` is preinstalled and `APT::Periodic` is configured to refresh package lists daily and process eligible unattended upgrades.

Users may also use Plasma Discover and manually update with:

```bash
sudo apt update
sudo apt upgrade
```

On the temporary **live session**, all installed updates and files normally disappear after reboot unless persistence is configured. On a **future installed system**, Debian packages will be able to update persistently.

## ZuzifyOS release images

The GitHub Actions `linux-nightly` release publishes a complete new experimental ISO after a successful main branch build. It is intended to be downloaded and reattached in a virtual machine; **it does not silently replace an existing operating system**.

An automatic ZuzifyOS system-image updater will require tested installed-system partitioning, signed update metadata, staging and rollback. That is planned, not implemented.

Never flash a nightly image over your PC's existing Windows installation.
