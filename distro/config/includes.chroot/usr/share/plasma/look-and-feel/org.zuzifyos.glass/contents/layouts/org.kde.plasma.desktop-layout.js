/*
 * ZuzifyOS Windows-style KDE Plasma 6 desktop layout.
 * This is a real native Linux taskbar with real application management.
 * No proprietary Windows code, images, or APIs are included.
 */
var plasma = getApiVersion(1);

// One bottom taskbar, Start menu, running programs, system tray, and clock.
var taskbar = new Panel();
taskbar.location = "bottom";
taskbar.height = Math.round(gridUnit * 2.7);
taskbar.floating = true;
taskbar.alignment = "center";
taskbar.hiding = "none";
taskbar.lengthMode = "fill";

var start = taskbar.addWidget("org.kde.plasma.kickoff");
start.currentConfigGroup = ["General"];
start.writeConfig("icon", "zuzify-start");

// Plasma's icon task manager displays and switches genuine app windows.
var tasks = taskbar.addWidget("org.kde.plasma.icontasks");
tasks.currentConfigGroup = ["General"];
tasks.writeConfig("launchers", [
    "applications:org.kde.dolphin.desktop",
    "applications:firefox-esr.desktop",
    "applications:org.kde.konsole.desktop",
    "applications:zuzify-windows-app.desktop",
    "applications:systemsettings.desktop"
].join(","));

// Leave flexible space for the status icons at the right side.
taskbar.addWidget("org.kde.plasma.panelspacer");
taskbar.addWidget("org.kde.plasma.systemtray");
taskbar.addWidget("org.kde.plasma.digitalclock");
taskbar.addWidget("org.kde.plasma.showdesktop");

for (var i = 0; i < desktops().length; ++i) {
    var desktop = desktops()[i];
    desktop.wallpaperPlugin = "org.kde.image";
    desktop.currentConfigGroup = ["Wallpaper", "org.kde.image", "General"];
    desktop.writeConfig("Image", "file:///usr/share/wallpapers/ZuzifyFlow/contents/images/1920x1080.png");
    desktop.writeConfig("FillMode", 2);
}
