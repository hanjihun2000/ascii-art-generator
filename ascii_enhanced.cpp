#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "malloc.h"
#include "memory.h"
#include "math.h"
#include "bmp.h"


#define MAX_SHADES 8

// The ascii code 160 represents '&nbsp' displaying on the webpage using HTML.
unsigned char shades[MAX_SHADES] = { '@','#','%','*','|','-','.',160 };

#define SAFE_FREE(p)  { if(p) { free(p); (p)=NULL; } }


int main(int argc, char** argv)
{
	//
	//	1. Open BMP file
	//
	Bitmap image_data(argv[2]);

	if (image_data.getData() == NULL)
	{
		printf("unable to load bmp image!\n");
		return -1;
	}

	
	//
	//	2. Obtain Luminance
	//
	int original_width = image_data.getWidth();
	int original_height = image_data.getHeight();
	unsigned char r, g, b;
	unsigned char* original_image = (unsigned char*)malloc((original_width * original_height) * sizeof(256));
	
	for (int y = 0; y < original_height; y++) 
	{
		for (int x = 0; x < original_width; x++) 
		{
			image_data.getColor(x, y, r, g, b);
			original_image[x + y * original_width] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
		}
	}


	//
	//  3. Resize image
	//
	char* ptr = strtok(argv[3], ",");
	int limit_w = atoi(ptr);
	ptr = strtok(NULL, "\n");
	int limit_h = atoi(ptr);
	if (limit_h == 0) {
		limit_h = limit_w;
	}
	int block_w = (int)ceil(original_width/(double)limit_w);
	int block_h = (int)ceil(original_height/(double)limit_h);
	int resized_width = original_width / block_w;
	int resized_height = original_height / block_h;
	unsigned char* resized_image = (unsigned char*)malloc((resized_width * resized_height) * sizeof(256));
	
	for (int y = 0; y < resized_height; y++) 
	{
		for (int x = 0; x < resized_width; x++) 
		{
			int value_sum = 0;
			for (int j = 0; j < block_h; j++) 
			{
				for (int i = 0; i < block_w; i++) 
				{
					value_sum += original_image[i + (j * original_width) + (x * block_w) + (y * block_h * original_width)];
				}
			}
			resized_image[x + y * resized_width] = value_sum / (block_h * block_w);
		}
	}

	
	//
	//	4. Quantization
	//
	for (int y=0; y<resized_height; y++) 
	{
		for (int x=0; x<resized_width; x++) 
		{
			resized_image[x+y*resized_width] /= (256 / 8);
		}
	}


	//
	//  5. ASCII Mapping on HTML
	//
	FILE* foutput = fopen(argv[4], "w");
	fputs("<html><head><title>Colored ASCII Art!</title></head><body bgcolor=#FFFFFF><font face=\"Courier New\">", foutput);
	for (int y = 0; y < resized_height; y++) {
		for (int x = 0; x < resized_width; x++) {
			image_data.getColor(x * block_w, y * block_h, r, g, b);
			fprintf(foutput, "<span style=\"color: rgb(%d, %d, %d);\">", r, g, b);
			if (argv[1][0] == 'p') {
				fputc(shades[resized_image[x + y * resized_width]], foutput);
			}
			else if (argv[1][0] == 's') {
				fputc(shades[7 - resized_image[x + y * resized_width]], foutput);
			}
			fputs("</span>", foutput);
		}
		fputs("<br>", foutput);
	}
	fputs("</body></html>", foutput);
	fclose(foutput);
	free(foutput);
	

	//
	//  6. Free memory
	//
	SAFE_FREE(original_image)
	SAFE_FREE(resized_image)
	return 0;
}
