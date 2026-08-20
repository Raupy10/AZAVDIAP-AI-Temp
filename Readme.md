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
### On Windows:
- **MinGW-w64** - Includes "make" and "g++"

## Installation

Use the following commands:

> The Windows version may not work probably since the last Update. This will be fixed with the full release.

### To make and build:
```bash
make all
```
### To just build:
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

### Setup
- **Select your default input and output device** and test if they're working.
> **If on Windows:** make sure your input and output devices are set as default for communication.

- **Start importing sounds** and assign a keybind to them.

- **Set the input of your desired program to "Output (AZAVDIAP)"**, test if the program detects your mic and sounds
  and adjust the volume of AZAVDIAP.
> **If on Windows:** if you don't hear anything while using AZAVDIAP check if your output device is set as default for communication
  and do the same for your microphone if your desired program doesn't detect you speaking.

**And with that you should be good to go!**

### Donations
Here is our Buy Me A Coffe link for donations!
<a href="https://buymeacoffee.com/teamnightfall" target="_blank">
  <img src="https://buymeacoffee.com/assets/img/custom_images/orange_img.png"
       alt="Buy Me A Coffee">
