# Plug64

Plug64 is a collection of simple audio plugins supporting up to 64 channels and per-channel operations. Plugins are available in VST3/LV2/AU formats and as standalone, for Windows, Linux and macOS. The motivation behind these plugins was the need, for my personal projects, of processors able to do some simple operations (like adjusting the gain or applying a filter) on an arbitrary number of channels, without having to load any big CPU-intensive plugin.
Moreover, these plugins allow not only to apply a master effect on all the channels, but to apply the same effect on each channel with different values. All the parameters are of course automatable.

## Included plugins

Other plugins coming soon!

### Gain64

A gain adjustment plugin.

### Filter64

A ladder filter with adjustable resonance (up to self-oscillation) and drive. Six different modes (lowpass, bandpass and highpass, each with 12 dB or 24 dB slope) can be chosen.

## Pre-built binaries

Coming soon!

## How to build

Grab the source with `git clone https://github.com/valeriorlandini/plug64.git`

`cd plug64` and then create the necessary build files with:
* `cmake -S . -B build -G "Visual Studio 17 2022"` on Windows (adjust the Visual Studio version if you have an older one.)
* `cmake -S . -B build -G "Unix Makefiles"` on Linux
* `cmake -S . -B build -G Xcode` on Mac

Navigate to the build folder with `cd build`

Next run `cmake --build . --config Release`

The compiled binaries can be found inside the various `PluginName/PluginName_artefacts/Release` (or simply `PluginName/PluginName_artefacts` in Linux) folder, with `PluginName` being the name of each available plugin.
