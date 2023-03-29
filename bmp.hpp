#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct pixel{
	pixel(int _r, int _g, int _b){
		r = _r;
		g = _g;
		b = _b;
	}
	pixel(int brightness){
		r = brightness;
		g = brightness;
		b = brightness;
	}
	pixel(std::string hex) {
		int _r, _g, _b;
		if (std::sscanf(hex.std::string::c_str(), "#%02x%02x%02x", &_r, &_g, &_b) != 3) throw 6;
		r = _r;
		g = _g;
		b = _b;
	}
	unsigned char r,g,b;
};

pixel operator +(pixel a,pixel b);

struct imgdata{
	imgdata(int _width,int _height){
		width = _width;
		height = _height;
		data = std::vector<std::vector<pixel>>(width, std::vector<pixel>(height,pixel(0)));
	}
	imgdata() {
		width = 0;
		height = 0;
		data = std::vector<std::vector<pixel>>(width, std::vector<pixel>(height, pixel(0)));
	}
	int width;
	int height;
	std::vector<std::vector<pixel>> data;
};

class bmp
{
public:
	bmp(std::string _filename);
	std::string filename;
	bool writeFile(imgdata imgdata);
private:
	std::ofstream file;
	imgdata data;
	int width, height;
	int filesize;
	const int bmpheadersize = 14;
	const int dibheadersize = 40;
	int rowWidthBytes;
	int arraysize;

	template<typename T>
	void write(T val) {
		file.write((char*)&val, sizeof(T));
	}
};
