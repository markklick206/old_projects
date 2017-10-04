//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
//#include "cuda_helper_funcs.h"
//#include "RGB.h"
//
///**
//* Helper function to calculate the greyscale value based on R, G, and B
//*/
//__device__ int greyscale(BYTE red, BYTE green, BYTE blue)
//{
//	int grey = 0.3 * red + 0.59 * green + 0 * 11 * blue; // calculate grey scale
//	return min(grey, 255);
//}
//
///**
//* Kernel for executing on GPY
//*/
//__global__ void greyscaleKernel(RGB *d_pixels, int height, int width)
//{
//	int x = blockIdx.x * blockDim.x + threadIdx.x; // width index
//	int y = blockIdx.y * blockDim.y + threadIdx.y; // height index
//
//	if (y >= height || y >= width) // thread is not within image
//		return;
//
//	int index = y * width + x;
//
//	int grey = greyscale(d_pixels[index].red, d_pixels[index].green, d_pixels[index].blue); // calculate grey scale
//
//	d_pixels[index].red = grey;
//	d_pixels[index].green = grey;
//	d_pixels[index].blue = grey;
//}
//
///**
//*	Host function for launching greyscale kernel
//*/
//__host__ void d_convert_greyscale(RGB *pixel, int height, int width)
//{
//	RGB *d_pixel;
//
//	cudaMalloc(&d_pixel, height * width * sizeof(RGB));
//	cudaMemcpy(d_pixel, pixel, height * width * sizeof(RGB), cudaMemcpyHostToDevice);
//
//	dim3 grid, block;
//	block.x = 16;
//	block.y = 16;
//	grid.x = calcBlockDim(width, block.x);
//	grid.y = calcBlockDim(height, block.y);
//
//	greyscaleKernel << <grid, block >> >(d_pixel, height, width);
//
//	cudaMemcpy(pixel, d_pixel, height * width * sizeof(RGB), cudaMemcpyDeviceToHost);
//}