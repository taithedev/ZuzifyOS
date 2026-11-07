#!/usr/bin/env python3
"""First-run welcome for ZuzifyOS. This is NOT a disk installer."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import subprocess
import sys

from PySide6.QtCore import Qt, QRectF, QPoint, QEasingCurve, QPropertyAnimation
from PySide6.QtGui import QColor, QFont, QLinearGradient, QPainter, QPen, QRadialGradient
from PySide6.QtWidgets import (
    QApplication, QFrame, QHBoxLayout, QLabel, QMainWindow, QPushButton,
    QStackedWidget, QVBoxLayout, QWidget,
)

FLAG = Path.home() / ".config" / "zuzifyos" / "welcome-done"
ACCENT = "#91a7ff"
MUTED = "#aebbd5"


def launch(command: list[str]) -> None:
    """Launch a known, fixed executable (never execute text as shell code)."""
    if shutil.which(command[0]):
        subprocess.Popen(command, start_new_session=True)


class GlassButton(QPushButton):
    def __init__(self, text: str, primary: bool = False):
        super().__init__(text)
        self.setCursor(Qt.CursorShape.PointingHandCursor)
        self.setMinimumHeight(45)
        self.setStyleSheet(
            "QPushButton { border: 1px solid "
            + ("#899bfa" if primary else "#526383")
            + "; border-radius: 15px; padding: 0 20px; font: 600 12pt 'Inter';"
            + f"color: #f2f5ff; background: {'#5064c1' if primary else 'rgba(68,85,121,160)'};"
            + " } QPushButton:hover {background: #7085e2; border-color: #b2c1ff;}"
            + "QPushButton:pressed {background: #43559f;}"
        )


class GlassWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("ZuzifyOS — Welcome")
        self.resize(840, 610)
        self.setMinimumSize(760, 570)
        self.setWindowFlags(Qt.WindowType.FramelessWindowHint | Qt.WindowType.Window)
        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground)
        self._drag: QPoint | None = None
        self.page_index = 0

        root = QWidget()
        self.setCentralWidget(root)
        outer = QVBoxLayout(root)
        outer.setContentsMargins(40, 34, 40, 34)
        outer.setSpacing(20)

        top = QHBoxLayout()
        mark = QLabel("◆")
        mark.setStyleSheet("font-size: 36px; color: #a0b5ff;")
        top.addWidget(mark)
        title = QLabel("ZUZIFY  OS")
        title.setStyleSheet("font: 700 20px 'Inter'; letter-spacing: 3px; color: white;")
        top.addWidget(title)
        top.addStretch(1)
        self.step = QLabel("01 / 04")
        self.step.setStyleSheet(f"font: 11pt 'Inter'; color: {MUTED};")
        top.addWidget(self.step)
        close = QPushButton("×")
        close.setFixedSize(42, 36)
        close.setCursor(Qt.CursorShape.PointingHandCursor)
        close.setStyleSheet(
            "QPushButton {color:#d5e0f7; font:22px 'Inter'; border:0;"
            "border-radius:12px; background:transparent}"
            "QPushButton:hover {background:#7b3c57;color:white;}"
        )
        close.clicked.connect(self.close)
        top.addWidget(close)
        outer.addLayout(top)

        self.stack = QStackedWidget()
        self.stack.setStyleSheet("background:transparent;")
        for maker in (self.welcome_page, self.desktop_page,
                      self.windows_page, self.finish_page):
            self.stack.addWidget(maker())
        outer.addWidget(self.stack, stretch=1)

        nav = QHBoxLayout()
        self.back_button = GlassButton("Back")
        self.back_button.clicked.connect(lambda: self.move_page(-1))
        nav.addWidget(self.back_button)
        nav.addStretch(1)
        self.next_button = GlassButton("Continue", True)
        self.next_button.clicked.connect(lambda: self.move_page(1))
        nav.addWidget(self.next_button)
        outer.addLayout(nav)
        self.sync_navigation()

        self.setWindowOpacity(0.01)
        animation = QPropertyAnimation(self, b"windowOpacity", self)
        animation.setDuration(460)
        animation.setStartValue(0.01)
        animation.setEndValue(1.0)
        animation.setEasingCurve(QEasingCurve.Type.OutCubic)
        animation.start()
        self._animation = animation

    def make_page(self, eyebrow: str, heading: str, description: str):
        page = QWidget()
        layout = QVBoxLayout(page)
        layout.setContentsMargins(8, 32, 8, 16)
        layout.setSpacing(16)
        overline = QLabel(eyebrow.upper())
        overline.setStyleSheet(f"font:600 11pt 'Inter'; color:{ACCENT};")
        layout.addWidget(overline)
        h = QLabel(heading)
        h.setWordWrap(True)
        h.setStyleSheet("font: 700 31pt 'Inter'; color: #f7faff;")
        layout.addWidget(h)
        p = QLabel(description)
        p.setWordWrap(True)
        p.setStyleSheet(f"font: 12pt 'Inter'; color:{MUTED}; line-height:1.5;")
        layout.addWidget(p)
        layout.addSpacing(16)
        return page, layout

    def chip(self, text: str):
        chip = QLabel(text)
        chip.setStyleSheet(
            "QLabel {border-radius:14px; background: rgba(79,101,152,130);"
            "border:1px solid rgba(160,183,244,65); color:#e3edff;"
            "font: 11pt 'Inter'; padding:14px 17px;}"
        )
        chip.setMinimumHeight(51)
        return chip

    def welcome_page(self):
        page, layout = self.make_page(
            "Your new desktop",
            "The Zuzify experience.",
            "A real Debian Linux system with a calm, glass-inspired desktop. "
            "Modern applications, familiar controls, and your own visual identity.",
        )
        layout.addWidget(self.chip("◈   Real Linux kernel and hardware support"))
        layout.addWidget(self.chip("◈   KDE Plasma desktop with floating glass panels"))
        layout.addWidget(self.chip("◈   Compatible Windows apps using Wine"))
        layout.addStretch(1)
        return page

    def desktop_page(self):
        page, layout = self.make_page(
            "Look & feel",
            "Your desktop, your style.",
            "The floating dock, top bar, and rounded panels use KDE Plasma. "
            "Choose a Zuzify color palette below; you can change it again in Settings.",
        )
        for label, scheme in (
            ("Midnight  ·  blue-black glass", "ZuzifyMidnight"),
            ("Violet  ·  plum and purple", "ZuzifyViolet"),
            ("Ocean  ·  teal and midnight", "ZuzifyOcean"),
        ):
            b = GlassButton(label)
            b.clicked.connect(lambda checked=False, s=scheme: self.set_theme(s))
            layout.addWidget(b)
        layout.addWidget(self.chip("You can open KDE System Settings at any time."))
        layout.addStretch(1)
        return page

    def windows_page(self):
        page, layout = self.make_page(
            "Windows compatibility",
            "Open more of your apps.",
            "ZuzifyOS includes Wine, a compatibility layer for many Windows "
            "programs. Some .exe and .msi files work; Windows-only drivers, "
            "anti-cheat games, and certain applications will not.",
        )
        b = GlassButton("Choose a Windows .exe or .msi", primary=True)
        b.clicked.connect(
            lambda: launch(["/usr/local/bin/zuzify-run-exe"])
        )
        layout.addWidget(b)
        layout.addWidget(self.chip(
            "Wine is not a sandbox. Open applications only from sources you trust."
        ))
        layout.addStretch(1)
        return page

    def finish_page(self):
        page, layout = self.make_page(
            "All set",
            "Welcome to ZuzifyOS.",
            "Explore your applications, open the file manager, or start configuring "
            "your desktop. This live image is still experimental; testing it in "
            "VirtualBox will not replace Windows.",
        )
        actions = (
            ("Open File Manager", ["dolphin"]),
            ("Open Settings", ["systemsettings"]),
            ("Open Terminal", ["konsole"]),
        )
        for label, command in actions:
            b = GlassButton(label)
            b.clicked.connect(lambda checked=False, c=command: launch(c))
            layout.addWidget(b)
        layout.addStretch(1)
        layout.addWidget(self.chip(
            "The live USB session is temporary. No disk installer is enabled yet."
        ))
        return page

    def set_theme(self, name: str):
        launch(["plasma-apply-colorscheme", name])

    def move_page(self, amount: int):
        target = self.page_index + amount
        if target >= self.stack.count():
            FLAG.parent.mkdir(parents=True, exist_ok=True)
            FLAG.write_text("ZuzifyOS Linux first-run walkthrough completed.\n")
            self.close()
            return
        self.page_index = max(0, target)
        self.stack.setCurrentIndex(self.page_index)
        self.sync_navigation()

    def sync_navigation(self):
        self.step.setText(f"0{self.page_index + 1} / 04")
        self.back_button.setVisible(self.page_index != 0)
        self.next_button.setText(
            "Start exploring" if self.page_index == 3 else "Continue"
        )

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        outer_rect = QRectF(7, 7, self.width() - 14, self.height() - 14)
        gradient = QLinearGradient(0, 0, 0, self.height())
        gradient.setColorAt(0, QColor(25, 37, 68, 246))
        gradient.setColorAt(0.55, QColor(14, 26, 50, 242))
        gradient.setColorAt(1, QColor(11, 20, 41, 250))
        painter.setBrush(gradient)
        painter.setPen(QPen(QColor(188, 204, 255, 90), 1.5))
        painter.drawRoundedRect(outer_rect, 29, 29)
        glow = QRadialGradient(self.width() * 0.80, self.height() * 0.17,
                               self.width() * 0.45)
        glow.setColorAt(0, QColor(109, 140, 247, 55))
        glow.setColorAt(1, QColor(109, 140, 247, 0))
        painter.setBrush(glow)
        painter.setPen(Qt.PenStyle.NoPen)
        painter.drawRoundedRect(outer_rect, 29, 29)
        super().paintEvent(event)

    def mousePressEvent(self, event):
        if event.button() == Qt.MouseButton.LeftButton and event.position().y() < 84:
            self._drag = event.globalPosition().toPoint() - self.frameGeometry().topLeft()
        super().mousePressEvent(event)

    def mouseMoveEvent(self, event):
        if self._drag is not None and event.buttons() & Qt.MouseButton.LeftButton:
            self.move(event.globalPosition().toPoint() - self._drag)
        super().mouseMoveEvent(event)

    def mouseReleaseEvent(self, event):
        self._drag = None
        super().mouseReleaseEvent(event)


def main() -> int:
    if FLAG.exists() and "--show" not in sys.argv[1:]:
        return 0
    app = QApplication(sys.argv)
    app.setApplicationDisplayName("ZuzifyOS Welcome")
    app.setStyle("Fusion")
    window = GlassWindow()
    window.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
