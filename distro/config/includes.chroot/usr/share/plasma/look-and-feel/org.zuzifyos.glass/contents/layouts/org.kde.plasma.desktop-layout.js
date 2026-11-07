/*
 * ZuzifyOS native Plasma 6 desktop layout.
 * Uses documented Plasma scripting API; no Apple binaries/assets.
 */
var plasma = getApiVersion(1);

// Keep the live environment bootable even if an optional widget is missing.
var top = new Panel();
top.location = "top";
top.height = Math.round(gridUnit * 2.05);
top.floating = true;
top.alignment = "center";
top.hiding = "none";

var menu = top.addWidget("org.kde.plasma.kickoff");
menu.currentConfigGroup = ["General"];
menu.writeConfig("icon", "start-here-kde");

top.addWidget("org.kde.plasma.appmenu");
top.addWidget("org.kde.plasma.panelspacer");
top.addWidget("org.kde.plasma.systemtray");
top.addWidget("org.kde.plasma.digitalclock");

// Plasma 6 floating panel provides the native blurred glass-like dock.
var dock = new Panel();
dock.location = "bottom";
dock.height = Math.round(gridUnit * 3.2);
dock.floating = true;
dock.alignment = "center";
dock.hiding = "none";
dock.minimumLength = 560;
dock.maximumLength = 1050;
dock.lengthMode = "fit";

var dockMenu = dock.addWidget("org.kde.plasma.kickoff");
dockMenu.currentConfigGroup = ["General"];
dockMenu.writeConfig("icon", "start-here-kde");

var tasks = dock.addWidget("org.kde.plasma.icontasks");
tasks.currentConfigGroup = ["General"];
tasks.writeConfig("launchers", [
  "applications:org.kde.dolphin.desktop",
  "applications:org.kde.konsole.desktop",
  "applications:firefox-esr.desktop",
  "applications:systemsettings.desktop",
  "applications:zuzify-windows-app.desktop"
].join(","));

for (var i = 0; i < desktops().length; ++i) {
    var desktop = desktops()[i];
    desktop.wallpaperPlugin = "org.kde.image";
    desktop.currentConfigGroup = ["Wallpaper", "org.kde.image", "General"];
    desktop.writeConfig("Image", "file:///usr/share/wallpapers/ZuzifyFlow/contents/images/1920x1080.png");
    desktop.writeConfig("FillMode", 2);
}
