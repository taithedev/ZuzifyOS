#!/usr/bin/env bash
set -Eeuo pipefail
cd "$(dirname "$0")/.."
if [[ "$(id -u)" != "0" ]]; then
    echo "ERROR: live-build must run as root inside a disposable Linux build environment." >&2
    echo "See distro/README.md; do not run this on your daily-use computer without reading it." >&2
    exit 1
fi
command -v lb >/dev/null || { echo "live-build is not installed" >&2; exit 1; }
command -v rsvg-convert >/dev/null || { echo "librsvg2-bin is required" >&2; exit 1; }
mkdir -p config/includes.chroot/usr/share/wallpapers/ZuzifyFlow/contents/images build
rsvg-convert --width 1920 --height 1080 assets/wallpaper.svg     --output config/includes.chroot/usr/share/wallpapers/ZuzifyFlow/contents/images/1920x1080.png
./auto/config
lb build 2>&1 | tee build/build.log
iso=""
for candidate in live-image-amd64.hybrid.iso live-image-amd64.iso live-image-amd64.hybrid.img; do
    if [[ -f "$candidate" ]]; then iso="$candidate"; break; fi
done
if [[ -z "$iso" ]]; then
    echo "ERROR: live-build finished without a recognized amd64 ISO image" >&2
    exit 1
fi
mv "$iso" build/ZuzifyOS-Linux-amd64.iso
sha256sum build/ZuzifyOS-Linux-amd64.iso > build/SHA256SUMS.txt
ls -lh build/ZuzifyOS-Linux-amd64.iso
