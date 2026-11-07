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
- **Set the microphone of your game to the virtual device** ("Monitor of AZAVDIAP" on Linux, "CABLE Output" on Windows).
  The game always receives your microphone and all sounds mixed together.
- **Test input** lets you hear yourself while the test runs, **test output** plays a short tone on your output.
- **"Ich höre"** decides what you hear on your own output: microphone + sounds, only sounds, only microphone or nothing.

### Sounds (tab "Soundboard")
- **+** adds a sound entry. Click the sound field to pick an mp3, click the key field and press a key combination
  (Ctrl/Alt/Shift + A-Z, 0-9 or F1-F24) to set the hotkey. Backspace/Delete clears the hotkey.
- Play/pause and trash buttons per entry. The list scrolls with the mouse wheel.
- Sounds are decoded once on first use and kept in memory until the program is closed.

### Settings
- **Global hotkeys** on/off (on by default).
- **Sound folder** and **"Nach MP3 suchen"** to add all mp3 files in that folder (and subfolders) at once.

### Stream Deck
Add a **Hotkey** action to a Stream Deck button and send the same hotkey as the sound (for example F13-F24,
which games normally don't use). No plugin is needed.

### Donations
Here is our Buy Me A Coffe link for donations!
<a href="https://buymeacoffee.com/teamnightfall" target="_blank">
  <img src="https://buymeacoffee.com/assets/img/custom_images/orange_img.png"
       alt="Buy Me A Coffee">
