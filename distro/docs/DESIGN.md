# Zuzify Glass desktop

## Experience

**Objective:** a complete Windows-style daily-use desktop experience using a real KDE Plasma 6 session, with original ZuzifyOS branding. No proprietary Microsoft or Apple code or artwork is included.

- KDE Plasma 6 on the real Debian Linux kernel.
- One translucent bottom taskbar with an original Zuzify Start icon, native application launcher, running applications, tray, clock, and Show Desktop control.
- An icon task manager with launchers for Files, Browser, Terminal, Run Windows App, and Settings.
- Dark background, soft aurora ribbon wallpaper, and accents that change with a Zuzify color scheme.
- KDE/KWin blur, contrast and animation options, plus rounded Breeze window decorations and Plasma floating panels.
- First-run Qt6 welcome window with controls, fade-in, original branding and color selection.

## Reality check

KWin blurs and translucency are hardware/compositor dependent. Not every application can have arbitrarily rounded content corners. The current implementation does **not** reproduce the Windows kernel, shell APIs or proprietary Windows drivers. Wine compatibility is per-application. The startup layout and effects still need visual validation after the actual ISO boots in a VM.

## Files

- `config/includes.chroot/usr/share/plasma/look-and-feel/org.zuzifyos.glass` — Plasma global theme and panel layout
- `config/includes.chroot/usr/share/color-schemes` — Midnight/Violet/Ocean color schemes
- `config/includes.chroot/etc/skel/.config` — defaults for newly created users
- `assets/wallpaper.svg` — original vector wallpaper
- `config/includes.chroot/usr/local/lib/zuzifyos/welcome.py` — PySide6 first-run UI

For desktop customization after boot, use **System Settings → Colors & Themes / Window Decorations / Desktop Effects**. Accessibility and performance preferences can disable transparency and animation.
