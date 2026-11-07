#!/usr/bin/env python3
"""Static repository validation (does not claim an ISO can boot)."""
from pathlib import Path
import configparser
import json
import subprocess
import sys
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
INCLUDE = ROOT / "config" / "includes.chroot"
required = {
    "auto/config": ROOT / "auto" / "config",
    "package list": ROOT / "config/package-lists/zuzify-desktop.list.chroot",
    "wallpaper": ROOT / "assets/wallpaper.svg",
    "Plasma theme": INCLUDE / "usr/share/plasma/look-and-feel/org.zuzifyos.glass/metadata.json",
    "Plasma layout": INCLUDE / "usr/share/plasma/look-and-feel/org.zuzifyos.glass/contents/layouts/org.kde.plasma.desktop-layout.js",
    "Windows app launcher": INCLUDE / "usr/local/bin/zuzify-run-exe",
    "welcome application": INCLUDE / "usr/local/lib/zuzifyos/welcome.py",
}
for name, file in required.items():
    if not file.is_file():
        sys.exit(f"ERROR: missing {name}: {file}")
    print(f"OK: {name}")
ET.parse(ROOT / "assets/wallpaper.svg")
metadata = json.loads(required["Plasma theme"].read_text())
assert metadata["KPackageStructure"] == "Plasma/LookAndFeel"
assert metadata["KPlugin"]["Id"] == "org.zuzifyos.glass"
packages = [
    x.split("#", 1)[0].strip()
    for x in required["package list"].read_text().splitlines()
]
packages = [x for x in packages if x]
assert len(packages) == len(set(packages)), "duplicate package"
for mandatory in ("plasma-desktop", "wine", "wine64", "sddm", "linux-image-amd64", "live-boot"):
    assert mandatory in packages, f"missing {mandatory}"
for path in (ROOT / "auto/config", ROOT / "scripts/build.sh",
             INCLUDE / "usr/local/bin/zuzify-run-exe",
             INCLUDE / "usr/local/bin/zuzify-welcome"):
    subprocess.run(["bash", "-n", str(path)], check=True)
for py in INCLUDE.glob("usr/local/lib/zuzifyos/*.py"):
    compile(py.read_text(), str(py), "exec")
for desktop in INCLUDE.glob("usr/share/applications/zuzify*.desktop"):
    cp = configparser.ConfigParser(interpolation=None)
    cp.read(desktop)
    assert cp.has_section("Desktop Entry") and cp.has_option("Desktop Entry", "Exec")
print(f"PASS: {len(packages)} packages and desktop assets validated.")
