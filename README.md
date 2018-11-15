# obs-qtwebengine
Open Broadcast Software Studio browser plugin

# Introduction
With Qt 5, WebKit was deprecated and the Qt developers switched to the Chromium web render engine. The WebKit code will still be avaliable but there will be no features added.
There are already 3 browser plugins for OBS Studio.
1. obs-qtwebkit
2. obs-linuxbrowser (for Linux)
3. obs-browser (for Windows)

And now there is the fourth obs-qtwebengine.
I initially created this project, becouse I had problems using obs-linuxbrowser with the new amdgpu driver.

## Features
Curently obs-qtwebengine support
1. Displaying Web-Pages
2. Interactive browsing (You can open another Browserwindow, to click links)

## Problems/Bugs
1. Currently it's a little slow. This is becouse of caching and various other settings who need to be changed.
2. Injecting CSS currently does not work. Since qt uses Chromium, this process has changed.
3. Hardware acceleration is untested.

# Building/Compiling

```bash
cd sourcedir
mkdir -p build; cd build
cmake -DCMAKE_INSTALL_PREFIX=obsprefix -DQT_PREFIX=qtprefix -DOBS_PREFIX=obsprefix ../
make
make install
```
