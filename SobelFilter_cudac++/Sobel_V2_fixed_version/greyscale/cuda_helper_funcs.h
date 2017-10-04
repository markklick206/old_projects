#ifndef _CUDA_HELPER_H_
#define _CUDA_HELPER_H_

/**
*	Helper function to calculate the number of blocks on an axis based on the total grid size and number of threads in that axis
*/
static inline int calcBlockDim(int total, int num_threads)
{
	int r = total / num_threads;
	if (total % num_threads != 0) // add one to cover all the threads per block
		++r;
	return r;
}

#endif