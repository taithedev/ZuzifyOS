# Zuzify Glass desktop

## Experience

**Objective:** a refined desktop inspired by modern macOS interaction patterns, without copying Apple branding, artwork, icons or code.

- KDE Plasma 6 on the real Debian Linux kernel.
- A translucent top bar with an application menu, tray and clock.
- A floating bottom dock using Plasma icon tasks for running windows and favorite applications.
- Dark background, soft aurora ribbon wallpaper, and accents that change with a Zuzify color scheme.
- KDE/KWin blur, contrast and animation options, plus rounded Breeze window decorations and Plasma floating panels.
- First-run Qt6 welcome window with controls, fade-in, original branding and color selection.

## Reality check

KWin blurs and translucency are hardware/compositor dependent. Not every application can have arbitrarily rounded content corners. The current implementation does **not** reproduce the proprietary macOS compositor, Finder, System Settings or Apple APIs. The startup layout and effects still need visual validation after the actual ISO boots in a VM.

## Files

- `config/includes.chroot/usr/share/plasma/look-and-feel/org.zuzifyos.glass` — Plasma global theme and panel layout
- `config/includes.chroot/usr/share/color-schemes` — Midnight/Violet/Ocean color schemes
- `config/includes.chroot/etc/skel/.config` — defaults for newly created users
- `assets/wallpaper.svg` — original vector wallpaper
- `config/includes.chroot/usr/local/lib/zuzifyos/welcome.py` — PySide6 first-run UI

For desktop customization after boot, use **System Settings → Colors & Themes / Window Decorations / Desktop Effects**. Accessibility and performance preferences can disable transparency and animation.
