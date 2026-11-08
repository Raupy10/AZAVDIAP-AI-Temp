# AZAVDIAP (Ausgabeprogramm zum Abspielen von Dateien in anderen Programmen)
> **English** = PFPMIOE (Program For Playing Media In Other Executables)

**AZAVDIAP is downloadable but it's still in early beta and not really usable!**

**If you want to get notified when it's releasing give us a star and watch this repo**

**Also please consider a little donation to help us develop more cool stuff in the future!**
<a href="https://buymeacoffee.com/teamnightfall" target="_blank">
  <img src="https://buymeacoffee.com/assets/img/custom_images/orange_img.png"
       alt="Buy Me A Coffee">
</a>

AZAVDIAP is a free and lightweight soundboard designed to play sounds for your games and other programs.
it uses a clean interface with as less junk as possible to ensure a great experience without evapourating your RAM.


## Dependencies

### For all operating systems
- **git** - Used to download Raylib
- **cmake** -  Used to build and configure the project

### On Linux:
- **pkg-config** - To detect X11 or Wayland
- **make** - Used to run CMake builds
- **g++** - Used to run CMake builds
- **zenity** - Used to get FilePath Input
- **pactl** (PulseAudio / PipeWire) - Creates the virtual output "AZAVDIAP" automatically
- **X11** - Global hotkeys work on X11 (and XWayland)
### On Windows:
- **MinGW-w64** - Includes "make" and "g++"
- **VB-Audio Virtual Cable** (or similar) - Provides the virtual output, detected automatically as "CABLE Input"

## Installation

Use the following commands:

> The Windows version may not work probably since the last Update. This will be fixed with the full release.

### To make and build:
```bash
make all
```
### To just build:
```bash 
make build 
``` 
### To clean build files and deps
```bash
make clean
```
### To only run 
```bash
make run
```

## Usage

### Setup (tab "Mikrofon & Ausgabe")
- **Choose input (microphone), output (headphones) and virtual output.** Every list has a system default entry.
  The virtual output is detected automatically ("AZAVDIAP" on Linux, "CABLE Input" on Windows).
- **Set the microphone of your game to the virtual device.** The program shows the exact name at the bottom of the tab
  ("Monitor of AZAVDIAP" on Linux, "CABLE Output (VB-Audio Virtual Cable)" on Windows).
  The game always receives your microphone and all sounds mixed together, you don't hear this channel.
  If the game has no microphone selection, set that device as the Windows default recording device.
- **Windows needs a driver for a virtual microphone.** Install the free [VB-CABLE](https://vb-audio.com/Cable/)
  once and restart Windows. The program detects it automatically and offers a download button until it is found.
- **Test input** lets you hear yourself while the test runs, **test output** plays a short tone on your output.
- **"Ich höre"** decides what you hear on your own output: microphone + sounds, only sounds, only microphone or nothing.

### Sounds (tab "Soundboard")
- **+** adds a sound entry. Click the sound field to pick an mp3, click the key field and press a key combination
  (Ctrl/Alt/Shift + A-Z, 0-9 or F1-F24) to set the hotkey. Backspace/Delete clears the hotkey.
- Play/pause and trash buttons per entry. The list scrolls with the mouse wheel.
- Sounds are decoded once on first use and kept in memory until the program is closed.

### Settings
- **Global hotkeys** on/off (on by default).
- **Pressing a hotkey again** while its sound plays: *Überlagern* (starts it again on top, default),
  *Neu starten* (stops that sound and restarts it) or *Ein / Aus* (second press stops it).
- **Alle Sounds stoppen**: its own hotkey (click the field, press the keys, Backspace clears it) plus a "Jetzt stoppen" button.
- **Sound folder** and **"Nach MP3 suchen"** to add all mp3 files in that folder (and subfolders) at once.

### Background mode (Windows)
Closing the window does not quit the program. It keeps running in the notification area next to the clock
(you may need to open the hidden icons). Left click the icon to open the window,
right click and **Beenden** to quit completely. Starting the program again just opens the running window.

### Stream Deck
Add a **Hotkey** action to a Stream Deck button and send the same hotkey as the sound (for example F13-F24,
which games normally don't use). No plugin is needed.

### Windows exe
Every push to `main` builds `AZAVDIAP.exe` (a single file, no extra files needed) with GitHub Actions.
Download it from the **latest** release or from the **Actions** tab (artifact "AZAVDIAP-windows").
If this repository is a fork, Actions must be enabled once under the **Actions** tab.

### Donations
Here is our Buy Me A Coffe link for donations!
<a href="https://buymeacoffee.com/teamnightfall" target="_blank">
  <img src="https://buymeacoffee.com/assets/img/custom_images/orange_img.png"
       alt="Buy Me A Coffee">
