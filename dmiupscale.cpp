//Requires modified version of lodepng. The encoder function must be modified to save the zTXt chunk before the IDAT chunk.
//	included with repo.
/*
 * How to use:
 * 1) Call with the filename of the dmi to convert as an arg. (Windows users can drag drop on to the executable)
 * 2) It will override the dmi with the doublely upscaled version.
 * 
 * How to compile:
 * Requires visual c++ compiler 2012 or any linux compiler with c++11 support.
 * Windows:
 *	Normal: cl.exe /EHsc /Ox /Qpar dmiupscale.cpp lodepng.cpp
 *	Debug: cl.exe /EHsc /Zi dmiupscale.cpp lodepng.cpp
 * Linux:
 *	Normal: g++ -O3 -std=c++11 dmiupscale.cpp lodepng.cpp -o dmiupscale
 *	Debug: g++ -g -Og -std=c++11 dmiupscale.cpp lodepng.cpp -o dmiupscale
 * Any Compile errors most likely indicate lack of c++11 support. Google how to upgrade or nag coderbus for help..
 */

#include "lodepng.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;
#define PIXEL_SIZE 4 //4 bytes per pixel.

int main(int argc, char *argv[])
{
	const char* filename = argc > 1 ? argv[1] : "input.dmi";
	vector<unsigned char> file;
	vector<unsigned char> png; //the raw pixels
	
	unsigned width, height;
	lodepng::State state; //png meta data.
	state.encoder.auto_convert = 0;
	unsigned error = lodepng::load_file(file, filename);
	if (!error) 
		error = lodepng::decode(png, width, height, state, file);
	file.clear(); //clear our file buffer
	//if there's an error, display it
	if(error) {
		cout << "decoder error " << error << ": "<< lodepng_error_text(error) << endl;
		exit(error);
	}
	unsigned long newwidth, newheight;
	newwidth = width*2;
	newheight = height*2;
	unsigned long l = width * width;
	unsigned long nl = newwidth * newwidth;
	vector<unsigned char> newpng;
	newpng.resize(nl*PIXEL_SIZE);
	unsigned long i = 0; 
	unsigned long o = 0; 
	unsigned long wx = width;
	while (i < l && o < nl) {
		if (!wx) {//end of row, copy the row downwards
			memcpy(&newpng.data()[o*PIXEL_SIZE], &newpng.data()[(o-newwidth)*PIXEL_SIZE], newwidth*PIXEL_SIZE);
			o += newwidth;
			wx = width;
		}
		//copy a pixel
		memcpy(&newpng.data()[o++*PIXEL_SIZE], &png.data()[i*PIXEL_SIZE], PIXEL_SIZE);
		//copy it again
		memcpy(&newpng.data()[o++*PIXEL_SIZE], &png.data()[i++*PIXEL_SIZE], PIXEL_SIZE);
		wx--;
	}
	string desc;
	bool found = false;
	size_t index = 0;
	for(size_t ii = 0; ii < state.info_png.text_num; ii++) {
		cout << state.info_png.text_keys[ii] << endl;
		if (strcmp(state.info_png.text_keys[ii], "Description") == 0) {
			desc = state.info_png.text_strings[ii];
			found = true;
			index = ii;
		}
			
	}
	if (!found)
		cout << "Warning: Could not find byond meta data chunk\n";
	size_t wi = desc.find("\twidth = ");
	if (found && wi == string::npos) {
		cout << "Warning: Could not find byond meta data for icon width.\n";
		found = false;
	}
	size_t hi = desc.find("\theight = ");
	if (found && hi == string::npos) {
		cout << "Warning: Could not find byond meta data for icon height.\n";
		found = false;
	}

	if (found) {
		hi += 10;
		wi += 9;
		size_t he = desc.find_first_of("\n\r \t", hi);
		size_t we = desc.find_first_of("\n\r \t", wi);
		
		int iconheight = stoi(desc.substr(hi, he-hi))*2;
		int iconwidth = stoi(desc.substr(wi, we-wi))*2;
		
		desc.replace(hi, he-hi, to_string(iconheight));
		desc.replace(wi, we-wi, to_string(iconwidth));
		
		char * cstr = new char[desc.size()+1];
		strcpy(cstr, desc.c_str());
		
		state.info_png.text_strings[index] = cstr;
	}

	error = lodepng::encode(file, newpng, newwidth, newheight, state);
	if(!error) lodepng::save_file(file, filename);
	if(error) {
		cout << "encoder error " << error << ": "<< lodepng_error_text(error) << endl;
		exit(error);
	}
}