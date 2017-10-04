/**
* CS-315 (Distributed Scalable Computing) Converting to greyscale + color average
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <thread>
#include <mutex>
#include <vector>
using namespace std;

#include "bmp.h"

#define NUM_IMAGES	3

/**
* CUDA device version of convert_greyscale
*/
void d_convert_greyscale(RGB *pixels, int height, int width);
/**
* CUDA device version of compute_component_average
*/
void d_compute_component_average(RGB *pixels, int height, int width); 
/**
* CUDA device version of compute_component_average
*/
void d_sobel_edge(RGB *pixels, int height, int width);

/**
* Converts each pixel from RGB to greyscale
*/
void convert_greyscale(RGB *pixels, int height, int width)
{
	for (int y = 0; y < height; ++y) { // for each row in image
		for (int x = 0; x < width; ++x) { // for each pixel in the row
			int index = y * width + x; // compute index position of (y,x) coordinate

			int grey = 0.3 * pixels[index].red + 0.59 * pixels[index].green + 0 * 11 * pixels[index].blue; // calculate grey scale

			pixels[index].red = min(grey, 255);
			pixels[index].green = min(grey, 255);
			pixels[index].blue = min(grey, 255);
		}
	}
}

/**
*  Parallel version of compute_component_average
*/
void p_compute_component_average(RGB *pixels, int height, int width)
{
	double total_red = 0, total_green = 0, total_blue = 0;

	int total_threads = 10; // Number of threads to spawn
	mutex lk; // Mutex used for protecting access to the shared variables total_red, total_green, and total_blue

	auto func = [&](int tid) { // Lambda function: each thread will spawn this
		double l_red = 0, l_green = 0, l_blue = 0; // Local variables for accumulating: keep it in the CPU cache

		// Portion of rows to work on for each thread: we are dividing up the work!
		int start = tid * (height / total_threads);
		int end;
		if (tid == total_threads - 1)
			end = height;
		else
			end = (tid + 1) * (height / total_threads);

		for (int y = start; y < end; ++y) { // each thread works on only a portion of rows
			for (int x = 0; x < width; ++x) { // for each pixel in the row
				int index = y * width + x; // compute index position of (y,x) coordinate
				l_red += pixels[index].red; // add the red value at this pixel to local accumulater
				l_green += pixels[index].green; // add the green value at this pixel to local accumulater
				l_blue += pixels[index].blue; // add the blue value at this pixel to local accumulater
			}
		}

		lock_guard<mutex> scope(lk); // get the mutex lock after the loop is complete
		total_green += l_green;
		total_red += l_red;
		total_blue += l_blue;
		// lock guard will release lock when it goes out of scope
	};

	vector<thread> pool; // Pool of threads

	for (int i = 0; i < total_threads; ++i)
		pool.push_back(thread(func, i)); // spawn each thread

	for (int i = 0; i < total_threads; ++i)
		pool[i].join(); // wait for each thread to complete

	cout << "Red average: " << total_red / (height*width) << endl;
	cout << "Green average: " << total_green / (height*width) << endl;
	cout << "Blue average: " << total_blue / (height*width) << endl;
}


/**
*  Computes the average of the red, green, and blue components of an image
*
* @param pixels  The array of RGB (Red, Green, Blue) components of each pixel in the image
* @param height  The height of the image
* @param width   The width of the image
*/
void compute_component_average(RGB *pixels, int height, int width)
{
	double total_red = 0, total_green = 0, total_blue = 0;

	for (int y = 0; y < height; ++y) { // each thread works on only a portion of rows
		for (int x = 0; x < width; ++x) { // for each pixel in the row
			int index = y * width + x; // compute index position of (y,x) coordinate
			total_red += pixels[index].red; // add the red value at this pixel to local accumulater
			total_green += pixels[index].green; // add the green value at this pixel to local accumulater
			total_blue += pixels[index].blue; // add the blue value at this pixel to local accumulater
		}
	}

	cout << "Red average: " << total_red / (height*width) << endl;
	cout << "Green average: " << total_green / (height*width) << endl;
	cout << "Blue average: " << total_blue / (height*width) << endl;
}

int main()
{
	do {
		string image_archive[NUM_IMAGES] = { "lena.bmp", "marbles.bmp", "sierra_02.bmp" };
		cout << "Select an image: \n";
		for (int i = 0; i < NUM_IMAGES; ++i)
			cout << i << ": " << image_archive[i] << endl;
		cout << NUM_IMAGES << ": exit\n";

		int choice;
		do {
			cout << "Please choice: ";
			cin >> choice;
			if (choice == NUM_IMAGES) {
				cout << "Goodbye!\n";
				exit(0);
			}
		} while (choice < 0 || choice > NUM_IMAGES);

		BitMap image(image_archive[choice]); // Load the bitmap image into the BitMap object

		// Display some of the image's properties
		cout << "Image properties\n";
		cout << setw(15) << left << "Dimensions: " << image.getHeight() << " by " << image.getWidth() << endl;
		cout << setw(15) << left << "Size: " << image.getImageSize() << " bytes\n";
		cout << setw(15) << left << "Bit encoding: " << image.getBitCount() << " bits\n\n";

		RGB *pixels = image.getRGBImageArray(); // get the image array of RGB (Red, Green, and Blue) components

		// Compute primary color component averages
		p_compute_component_average(pixels, image.getHeight(), image.getWidth());
		d_compute_component_average(pixels, image.getHeight(), image.getWidth());

		// Convert to greyscale
		//d_convert_greyscale(pixels, image.getHeight(), image.getWidth());

		// Sobel Edge Filtering Procedure
		d_sobel_edge(pixels, image.getHeight(), image.getWidth());

		image.setImageFromRGB(pixels); // Assign the modified pixels back to the image

		image.saveBMP("test.bmp"); // Save this image in "test.bmp"

		cout << "Check out test.bmp (click on it) to see image processing result\n\n";
		char response = 'y';
		cout << "Do you wish to repeat? [y/n] ";
		cin >> response;
		if (response != 'y') {
			cout << "Sorry to see you go ...\n";
			exit(0);
		}
	} while (true);
}