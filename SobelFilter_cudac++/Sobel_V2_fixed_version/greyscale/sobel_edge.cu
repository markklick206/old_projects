#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_helper_funcs.h"
#include "RGB.h"
#include <math.h>
#include <vector>

/**
* Helper function to calculate the greyscale value based on R, G, and B
*/
__device__ int greyscale(BYTE red, BYTE green, BYTE blue)
{
	int grey = 0.3 * red + 0.59 * green + 0 * 11 * blue; // calculate grey scale
	return min(grey, 255);
}

/**
* Kernel for executing on GPY
*/
__global__ void greyscaleKernel(RGB *d_pixels, int height, int width)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x; // width index
	int y = blockIdx.y * blockDim.y + threadIdx.y; // height index

	if (y >= height || y >= width) // thread is not within image
		return;

	int index = y * width + x;

	int grey = greyscale(d_pixels[index].red, d_pixels[index].green, d_pixels[index].blue); // calculate grey scale

	d_pixels[index].red = grey;
	d_pixels[index].green = grey;
	d_pixels[index].blue = grey;
}

/**
*	Host function for launching greyscale kernel
*/
__host__ void d_convert_greyscale(RGB *pixel, int height, int width)
{
	RGB *d_pixel;

	cudaMalloc(&d_pixel, height * width * sizeof(RGB)); // Allocate memory on device to store pixels
	cudaMemcpy(d_pixel, pixel, height * width * sizeof(RGB), cudaMemcpyHostToDevice); // Copy pixels to device

	dim3 grid, block;
	block.x = 16;
	block.y = 16;
	grid.x = calcBlockDim(width, block.x);
	grid.y = calcBlockDim(height, block.y);

	greyscaleKernel << <grid, block >> >(d_pixel, height, width);

	cudaMemcpy(pixel, d_pixel, height * width * sizeof(RGB), cudaMemcpyDeviceToHost); // Copy results back to the host
}

#define P(x,y)	d_pixels[(y)* width + (x)].blue
/**
* Helper function for computing the sobel value at pixel (x,y)
*/
__device__ int sobel_filter(int x, int y, RGB *d_pixels, int height, int width)
{
	//convolution 
	/*int Gx = -(d_pixels[(y - 1) * width + (x - 1)].blue) - 2 * (d_pixels[(y)* width + (x - 1)].blue) - (d_pixels[(y+1)* width + (x-1)].blue) 
		+ (d_pixels[(y + 1)* width + (x - 1)].blue) * (d_pixels[(y)* width + (x + 1)].blue) + (d_pixels[(y+1)* width + (x+1)].blue);

	int Gy = (d_pixels[(y - 1)* width + (x - 1)].blue) + 2 * (d_pixels[(y - 1)* width + (x)].blue) + (d_pixels[(y-1)* width + (x+1)].blue)
		- (d_pixels[(y + 1)* width + (x - 1)].blue) - 2 * (d_pixels[(y + 1)* width + (x)].blue) - (d_pixels[(y+1)* width + (x+1)].blue);
*/
	//Eds c++ macro trick much better

	int Gx = -P(x-1,y-1) - 2 * P(x-1,y) - P(x-1,y+1)
		+ P(x-1,y+1) + 2 * P(x+1,y) + P(x+1,y+1);
	
	int Gy = P(x - 1, y - 1) + 2 * P(x, y - 1) + P(x + 1, y - 1)
		- P(x - 1, y + 1) - 2 * P(x, y + 1) - P(x + 1, y + 1);
		
	int sum = abs(Gx) + abs(Gy);

	return min(sum,255);
}

/**
* Kernel for computing sobel filter 
*/
__global__ void sobelKernel(RGB *d_pixels, int height, int width)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x; // width index
	int y = blockIdx.y * blockDim.y + threadIdx.y; // height index

	if (y >= height || x >= width) // thread is not within image
		return;

	if (x > 0 && y > 0 && y < height - 1 && x < width - 1) { // valid sobel range
		d_pixels[(y)* width + (x)].green = sobel_filter(x, y, d_pixels, height, width); //store sobel values in the pixel's green variable
	}
}

/**
* SECOND!!! Kernel for updating red and blue components with the sobel filter values stored in green
*/
__global__ void componentUpdateKernel(RGB *d_pixels, int height, int width)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x; // width index
	int y = blockIdx.y * blockDim.y + threadIdx.y; // height index

	if (y >= height || x >= width) // thread is not within image
		return;

	if (x > 0 && y > 0 && y < height - 1 && x < width - 1) { // See sobelKernel
		d_pixels[(y)* width + (x)].red = d_pixels[(y)* width + (x)].green; // assign sobel value to red and blue variables 
		d_pixels[(y)* width + (x)].blue = d_pixels[(y)* width + (x)].green;
	}
}

/**
*	Host function for launching the sobel kernel
*/
__host__ void d_sobel_edge(RGB *pixel, int height, int width)
{
	RGB *d_pixel;

	cudaMalloc(&d_pixel, height * width * sizeof(RGB)); 
	cudaMemcpy(d_pixel, pixel, height * width * sizeof(RGB), cudaMemcpyHostToDevice); 

	dim3 grid, block;
	block.x = 16;
	block.y = 16;
	grid.x = calcBlockDim(width, block.x);
	grid.y = calcBlockDim(height, block.y);

	greyscaleKernel << <grid, block >> >(d_pixel, height, width); 
	sobelKernel << <grid, block >> >(d_pixel, height, width); 
	//second sobel kernel comes into play 
	componentUpdateKernel << <grid, block >> >(d_pixel, height, width);

	cudaMemcpy(pixel, d_pixel, height * width * sizeof(RGB), cudaMemcpyDeviceToHost); 

	cudaDeviceSynchronize();
}