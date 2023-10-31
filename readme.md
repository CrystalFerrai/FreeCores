## FreeCores

FreeCores is a simple Windows console app that updates the CPU affinity of running processes to prevent that process from using a specified number of physical processor cores.

## Releases

Release can be found [here](https://github.com/CrystalFerrai/FreeCores/releases).

## Installation

This program is released as a standalone executable. Before the program can run, you will need to have the x64 Visual C++ runtime from Microsoft installed on your system. The installer for this runtime can be downloaded here: [vc_redist.x64.exe](https://aka.ms/vs/17/release/vc_redist.x64.exe)

## Usage

The simplest way to use this is to create a batch script that looks something like the following. Replace anything in square brackets \[\] with your own information.

```
[Path to FreeCores]FreeCores.exe [free core count] [MyApp].exe
```

To see full usage notes, including all available options, run the program from a console window with no parmeters.

## Building

To build from source, just open the SLN file in Visual Studio 2022 and build it.

## Platforms Supported

**Windows Only**

FreeCores calls directly into Windows APIs. Other platforms have similar APIs, but only Windows has been implemented. Therefore, this library will not work on other platforms.