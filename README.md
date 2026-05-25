# Laser Keyboard

A sci-fi holographic laser keyboard and air mouse for Windows. The app overlays a
transparent, always-on-top window, tracks your hands from a webcam (via a Python
MediaPipe sidecar), and injects system-wide input. It has two modes: a keyboard
you type on by dwelling over glowing keys, and a mouse you drive with hand poses.
Hold an open palm for about a second to switch between them.

> **Status: work in progress.** This is a personal portfolio project under active
> development and is not production-ready. Expect rough edges and changing behavior.

## Project structure

- `CMakeLists.txt`: build configuration
- `assets/`: shaders and the MediaPipe hand model
- `src/`: application source code
- `scripts/tracker.py`: MediaPipe hand-tracking sidecar (streams landmarks over UDP)
- `scripts/dev-env.ps1`: sets up the MSVC + Windows SDK build environment

## Build

```powershell
. .\scripts\dev-env.ps1
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
cmake --build build
```

The tracker needs Python with `mediapipe` and `opencv-python` installed
(`pip install --user mediapipe opencv-python`). The app spawns it automatically.

## Run

Run `build/laser_keyboard.exe` from the project root (it loads `assets/` and the
tracker by relative path, so the working directory must be the repo root).

It starts in keyboard mode. Calibrate by holding your index fingertip on each
magenta target, then dwell over a key to type it. Dwell `CAPS` for uppercase.

Hold an **open palm** for about a second to toggle between keyboard and mouse mode.
In mouse mode: **point** (index finger) to move the cursor, **pinch** thumb and
index to click.

`F8` arms or disarms input injection (the badge shows the state), `F9` recalibrates,
`ESC` quits.
