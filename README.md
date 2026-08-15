# Libre_Player

Libre Player is a lightweight Qt6 desktop audio player.

- Playlist with drag-and-drop file adding and reordering

- Folder browsing with breadcrumb navigation

- Shuffle and repeat (off/all/one) modes

- Click on the bar for seeking

- Customizable keyboard shortcuts (via shortcuts.txt)

- Remembers volume, mute state, shuffle/repeat, last track, and window size/position between sessions

- Adjustable UI scaling

# Requires QT6 dev tools to built.

# Ubuntu/Debian
sudo apt install build-essential cmake qt6-base-dev qt6-multimedia-dev

# Fedora
sudo dnf install cmake qt6-qtbase-devel qt6-qtmultimedia-devel

# Arch
sudo pacman -S cmake qt6-base qt6-multimedia

# Clone it

git clone https://github.com/Procurador1337/Libre_Player.git
cd Libre_Player

# Configure and build

cmake -B build -S .
cmake --build build

# Default shortcuts:

PlayPause=Space
Stop=S
Previous=Left
Next=Right
SeekBackward=Ctrl+Left
SeekForward=Ctrl+Right
VolumeUp=Up
VolumeDown=Down
Mute=M
