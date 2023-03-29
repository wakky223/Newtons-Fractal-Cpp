#include "bmp.hpp"

#ifdef __DEBUG
#define assert(expr) if(!expr) std::cout << "Assert error on line " << __LINE__ << " in file " << __FILE__ << std::endl; throw __LINE__
#else 
#define assert(expr)
#endif

//add two pixels together with clamping at 255
pixel operator +(pixel a,pixel b) {
	pixel c(0);
	if (short(a.r) + short(b.r) > 255) {
		c.r = 255;
	}
	else {
		c.r = a.r + b.r;
	}
	if (short(a.g) + short(b.g) > 255) {
		c.g = 255;
	}
	else {
		c.g = a.g + b.g;
	}
	if (short(a.b) + short(b.b) > 255) {
		c.b = 255;
	}
	else {
		c.b = a.b + b.b;
	}
	return c;
}

/// @brief Write image data to the file
/// @param imgdata struct containing rgb values for each pixel
/// @return weather or not the file was saved sucessfully
bool bmp::writeFile(imgdata imgdata) {
	try{
		data = imgdata;
		width = imgdata.width;
		height = imgdata.height;
		if (width * 3 % 4 == 0) {
			rowWidthBytes = width * 3;
		}
		else {
			rowWidthBytes = width * 3 + (4 - (width * 3) % 4);
		}
		arraysize = rowWidthBytes * height;
		filesize = bmpheadersize + dibheadersize + arraysize;
		file.open("./images/" + filename, std::fstream::binary);
		
		//BMP Header
		write(short(0x4d42));			//ID field "BM"
		write(filesize);				//File size
		write(0);						//Unused
		write(int(54));					//Offset where the pixel array (bitmap data) can be found

		//DIB Header
		write(40);						//Number of bytes in the DIB header (from this point)
		write(width);					//Width of the bitmap in pixels
		write(height);					//Height of the bitmap in pixels
		write(short(1));				//Number of color panes
		write(short(24));				//Bits per pixel		
		write(0);						//Compression
		write(arraysize);				//Pixel array size
		write(2835);					//Horizontal print resolution
		write(2835);					//Vertical print resolution
		write(0);						//colors in palette
		write(0);						//important colors
		
		//pixel array
		assert(rowWidthBytes % 4 == 0);
		for (int i = height - 1; i >= 0; i--)
		{
			int j = 0;
			while (j < width) {
				write(data.data[j][i].b);
				write(data.data[j][i].g);
				write(data.data[j][i].r);
				j++;
			}
			j *= 3;
			while (j < rowWidthBytes) {
				write((unsigned char)(0));
				j++;
			}
		}
		file.close();
		return true;
	}
	catch(int exc){
		return false;
	}
}

bmp::bmp(std::string _filename)
{
	filename = _filename;
}


#undef assert