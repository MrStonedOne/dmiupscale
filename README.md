# DMI Upscale
Converts byond dmis to be exactly double their size. (also works on normal pngs)

## How to use:
1. Call with the filename of the dmi to convert as an arg. (Windows users can drag drop on to the executable)
2. It will override the dmi with the doublely upscaled version.

## How to compile:
Requires visual c++ compiler 2012 or any linux compiler with c++11 support.

Requires modified version of lodepng. The encoder function must be modified to save the zTXt chunk before the IDAT chunk since byond cares about the order. (included with repo)

### Windows:
 *	Normal: cl.exe /EHsc /Ox /Qpar dmiupscale.cpp lodepng.cpp
 *	Debug: cl.exe /EHsc /Zi dmiupscale.cpp lodepng.cpp

### Linux:
 *	Normal: g++ -O3 -std=c++11 dmiupscale.cpp lodepng.cpp -o dmiupscale
 *	Debug: g++ -g -Og -std=c++11 dmiupscale.cpp lodepng.cpp -o dmiupscale

Any Compile errors most likely indicate lack of c++11 support. Google how to upgrade or nag coderbus for help..
