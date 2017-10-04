#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_helper_funcs.h"
#include "RGB.h"

#include <iostream>
using namespace std;

// Block dimensions
#define X_BLOCK_SIZE	32
#define Y_BLOCK_SIZE	32

// Shared variables: fast memory for each block
__shared__ float red_share[X_BLOCK_SIZE][Y_BLOCK_SIZE], green_share[X_BLOCK_SIZE][Y_BLOCK_SIZE], blue_share[X_BLOCK_SIZE][Y_BLOCK_SIZE];

__global__ void accumulate_colors(RGB *d_pixels, int height, int width, float3 *total_colors)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x; // width index
	int y = blockIdx.y * blockDim.y + threadIdx.y; // height index

	if (y >= height || x >= width) // index is not withing image
		return;

	int index = y * width + x; // index into pixel buffer

	int totalThreads = blockDim.x;
	int me = threadIdx.x;

	red_share[me][threadIdx.y] = d_pixels[index].red;
	green_share[me][threadIdx.y] = d_pixels[index].green;
	blue_share[me][threadIdx.y] = d_pixels[index].blue;

	__syncthreads(); //language extension not actually a function

	while (totalThreads > 1){
		int buddy_line = totalThreads / 2;
		if (me < buddy_line){
			int buddy = me + buddy_line;
			red_share[me][threadIdx.y] += red_share[buddy][threadIdx.y];
			green_share[me][threadIdx.y] += green_share[buddy][threadIdx.y];
			blue_share[me][threadIdx.y] += blue_share[buddy][threadIdx.y];
		}
		__syncthreads();
		totalThreads /= 2;
	}

	if (me == 0){
		int myblock = blockIdx.x + blockIdx.y * gridDim.x;
		atomicAdd(&total_colors[myblock].x, red_share[me][threadIdx.y]);
		atomicAdd(&total_colors[myblock].y, green_share[me][threadIdx.y]);
		atomicAdd(&total_colors[myblock].z, blue_share[me][threadIdx.y]);
	}
}

__host__ void d_compute_component_average(RGB *pixel, int height, int width)
{
	RGB *d_pixel; // Part 1: pointer to array for storing image pixel on device

	cudaMalloc(&d_pixel, height * width * sizeof(RGB)); // Part 2: allocate space on device
	cudaMemcpy(d_pixel, pixel, height * width * sizeof(RGB), cudaMemcpyHostToDevice); // Part 3: copy image to device

	dim3 grid, block;
	block.x = X_BLOCK_SIZE;
	block.y = Y_BLOCK_SIZE;
	grid.x = calcBlockDim(width, block.x);
	grid.y = calcBlockDim(height, block.y);

	float3 *d_total_colors; // part 1: pointer to array for storing partial sums from each block
							// float3::x red
							// float3::y green
							// float3::z blue
	cudaMalloc(&d_total_colors, grid.x * grid.y * sizeof(float3)); // part 2: allocate space on device
	cudaMemset(d_total_colors, 0, grid.x * grid.y * sizeof(float3)); // part 3: initialize array to zero

	accumulate_colors << <grid, block >> >(d_pixel, height, width, d_total_colors);

	float3 *h_total_colors = new float3[grid.x * grid.y];	// allocate memory of host to collect partial sums from device
	cudaMemcpy(h_total_colors, d_total_colors, grid.x * grid.y * sizeof(float3), cudaMemcpyDeviceToHost); // copy partial sums from device

	double total_green = 0, total_red = 0, total_blue = 0;
	for (int i = 0; i < grid.x*grid.y; ++i) { // sum up all the partial sums returned from each block
		total_red += h_total_colors[i].x;
		total_green += h_total_colors[i].y;
		total_blue += h_total_colors[i].z;
	}

	// Display primary color averages
	cout << "Red average: " << total_red / (height*width) << endl;
	cout << "Green average: " << total_green / (height*width) << endl;
	cout << "Blue average: " << total_blue / (height*width) << endl;
}