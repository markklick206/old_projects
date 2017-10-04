/*
*	Matrix Multiplication
*	Serial Implementation of two variations of matrix multiplication
*	Parallelized versions of the serial methods using C++11 and OpenMP
*/
// @author Mark Klick
//
#include <iostream>
#include <cMath>
#include <ctiMe>
#include <ioManip>
#include <string>
#include <fstream>
#include <istream>
#include <sstream>
#include <algorithM>
#include <vector>
#include <thread>

#include <ppl.h>
#include <omp.h>
#include "Random.h"

#define MAT_SIZE 1000
#define NUM_THREADS 4
#define NUM_ROUNDS 20
//#define MATRIXX
#define TMATRIXX
#define SHOWME
//#define PRINT
Random My_random;

double ** Create_Matrix(int size){
	double** Mat = new double*[size];
	for (int i = 0; i < size; i++)
		Mat[i] = new double[size];
	return Mat;
}

void Fill_Matrix(double** Mat, int size){
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			Mat[i][j] = My_random.next_double();
		}
	}
}

void Destroy_Matrix(double** Mat, int size) {
	for (int i = 0; i < size; i++)
		delete[] Mat[i];
	delete[] Mat;
}

//SERIAL MATRIX MULITIPLY
void Matrix_Multiply(double** M1, double** M2, double** result, int size) {
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			double teMp = 0;
			for (int k = 0; k < size; k++)
			{
				teMp += M1[i][k] * M2[k][j];
			}
			result[i][j] = teMp;
		}
	}
}

//PARALLEL MATRIX MULTIPLY
void PMatrix_Multiply(double** M1, double** M2, double** result, int size) {
	concurrency::parallel_for(0, MAT_SIZE, [&](int tid) {
		for (int j = 0; j < size; j++)
		{
			double teMp = 0;
			for (int k = 0; k < size; k++)
			{
				teMp += M1[tid][k] * M2[k][j];
			}
			result[tid][j] = teMp;
		}
	});
}

//OMP MATRIX MULTIPLY
void Omp_Matrix_Multiply(double** M1, double** M2, double** result, int size) {
//#pragma omp parallel for num_threads(NUM_THREADS) schedule(static)
	for (int i = 0; i < size; i++)
	{
#pragma omp parallel num_threads(NUM_THREADS)
		for (int j = 0; j < size; j++)
		{
			double teMp = 0;
			for (int k = 0; k < size; k++)
			{
				teMp += M1[i][k] * M2[k][j];
			}
			result[i][j] = teMp;
		}
	}
}

//COMPUTES THE DOT PRODUCT
double Dot_Product(double* V1, double* V2, int size) {
	double teMp = 0.0;
	for (int i = 0; i < size; i++)
	{
		teMp += V1[i] * V2[i];
	}
	return teMp;
}

//SERIAL TRANSPOSE MATRIX MULTIPLY
void Transpose_Matrix_Multiply(double** M1, double** M2, double** result, int size) {
	
	double** M2_t = Create_Matrix(size);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			M2_t[j][i] = M2[i][j];
		}
	}
	
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			result[i][j] = Dot_Product(M1[i], M2_t[j], size);
		}
	}

	Destroy_Matrix(M2_t, size);
}

//PARALLEL TRANSPOSE MATRIX MULTIPLY
void PTranspose_Matrix_Multiply(double** M1, double** M2, double** result, int size) {
	
	double** M2_t = Create_Matrix(size);

	concurrency::parallel_for(0, MAT_SIZE, [&](int tid) {
		for (int j = 0; j < size; j++)
		{
			M2_t[j][tid] = M2[tid][j];
		}
	});

	concurrency::parallel_for(0, MAT_SIZE, [&](int tid) {
		for (int j = 0; j < size; j++)
		{
			result[tid][j] = Dot_Product(M1[tid], M2_t[j], size);
		}
	});

	Destroy_Matrix(M2_t, size);
}

//OMP TRANSPOSE MATRIX MULTIPLY
void Omp_Transpose_Matrix_Multiply(double** M1, double** M2, double** result, int size) {

	double** M2_t = Create_Matrix(size);

//#pragma omp parallel for num_threads(NUM_THREADS) schedule(static, 1000)

	for (int i = 0; i < size; i++)
	{
#pragma omp parallel num_threads(NUM_THREADS)
		for (int j = 0; j < size; j++)
		{
			M2_t[j][i] = M2[i][j];
		}
	}

	for (int i = 0; i < size; i++)
	{
#pragma omp parallel num_threads(NUM_THREADS)
		for (int j = 0; j < size; j++)
		{
			result[i][j] = Dot_Product(M1[i], M2_t[j], size);
		}
	}

	Destroy_Matrix(M2_t, size);
}

int main() {

	int i = NUM_ROUNDS;
	double Total_Run_Time = 0;

	while (i--) {
		//intialize Matrices
		double** Mat1 = Create_Matrix(MAT_SIZE); Fill_Matrix(Mat1, MAT_SIZE);
		double** Mat2 = Create_Matrix(MAT_SIZE); Fill_Matrix(Mat2, MAT_SIZE);
		double** Mat3 = Create_Matrix(MAT_SIZE); //Fill_Matrix(Mat3, MAT_SIZE);

		std::clock_t startTime = clock();
#ifdef MATRIXX
		Matrix_Multiply(Mat1, Mat2, Mat3, MAT_SIZE);
#endif

#ifdef TMATRIXX
		Transpose_Matrix_Multiply(Mat1, Mat2, Mat3, MAT_SIZE);
#endif
		std::clock_t endTiMe = clock();
		double End_Tick = omp_get_wtick();
		double Run_Time = double(endTiMe - startTime) / (double)CLOCKS_PER_SEC;
		Total_Run_Time += Run_Time;

#ifdef SHOWME
		std::cout << "Run Time: " << Run_Time << " seconds." << std::endl;
#endif
		//std::cout << "Clock Ticks: " << End_Tick << std::endl;
	}

	double Avg_Serial = Total_Run_Time / NUM_ROUNDS;

	int i1 = NUM_ROUNDS;
	double Total_Run_Time1 = 0;

	while (i1--) {
		//intialize Matrices
		double** Mat1 = Create_Matrix(MAT_SIZE); Fill_Matrix(Mat1, MAT_SIZE);
		double** Mat2 = Create_Matrix(MAT_SIZE); Fill_Matrix(Mat2, MAT_SIZE);
		double** Mat3 = Create_Matrix(MAT_SIZE); //Fill_Matrix(Mat3, MAT_SIZE);

		std::clock_t startTime = clock();

#ifdef MATRIXX
		PMatrix_Multiply(Mat1, Mat2, Mat3, MAT_SIZE);
#endif

#ifdef TMATRIXX
		PTranspose_Matrix_Multiply(Mat1, Mat2, Mat3, MAT_SIZE);
#endif
		std::clock_t endTiMe = clock();
		double End_Tick = omp_get_wtick();
		double Run_Time = double(endTiMe - startTime) / (double)CLOCKS_PER_SEC;
		Total_Run_Time1 += Run_Time;

#ifdef SHOWME
		std::cout << "Run Time: " << Run_Time << " seconds." << std::endl;
#endif

	}
	double Avg_Parrallel = Total_Run_Time1 / NUM_ROUNDS;
	double Par_Speedup = Avg_Serial / Avg_Parrallel;
			

	
	int i2 = NUM_ROUNDS;
	double Total_Run_Time2 = 0;

	while (i2--) {
		//intialize Matrices
		double** Mat1 = Create_Matrix(MAT_SIZE); Fill_Matrix(Mat1, MAT_SIZE);
		double** Mat2 = Create_Matrix(MAT_SIZE); Fill_Matrix(Mat2, MAT_SIZE);
		double** Mat3 = Create_Matrix(MAT_SIZE); //Fill_Matrix(Mat3, MAT_SIZE);

		std::clock_t startTime = clock();

#ifdef MATRIXX
		Omp_Matrix_Multiply(Mat1, Mat2, Mat3, MAT_SIZE);
#endif

#ifdef TMATRIXX
		Omp_Transpose_Matrix_Multiply(Mat1, Mat2, Mat3, MAT_SIZE);
#endif
		std::clock_t endTiMe = clock();
		double End_Tick = omp_get_wtick();
		double Run_Time = double(endTiMe - startTime) / (double)CLOCKS_PER_SEC;
		Total_Run_Time2 += Run_Time;

#ifdef SHOWME
		std::cout << "Run Time: " << Run_Time << " seconds." << std::endl;
#endif

	}

	double Avg_Omp = Total_Run_Time2 / NUM_ROUNDS;
	double Omp_Speedup = Avg_Serial / Avg_Omp;
	
	std::cout << "Average Serial Run Time: " << Avg_Serial << std::endl;
	std::cout << "Parallel Speed up: " << Par_Speedup << std::endl;
	std::cout << "Omp Speed up: " << Omp_Speedup << std::endl;
	std::cout << "Num processors: " << omp_get_num_procs() << std::endl;
	std::cout << "Max Threads: " << omp_get_max_threads() << std::endl;

#ifdef PRINT
	std::ofstream output1;
	output1.open("Speed_Up_TMat_1000.txt");
	output1 << "Average Serial Run Time: " << Avg_Serial << std::endl;
	output1 << "Parallel Speed up: " << Par_Speedup << std::endl;
	output1 << "Omp Speed up: " << Omp_Speedup << std::endl;
	output1.close();
#endif
}

/*std::ofstream output1;
output1.open("Matrix.txt");
output1 << std::endl;
for (int i = 0; i < MAT_SIZE; i++) {
	for (int j = 0; j < MAT_SIZE; j++) {
		output1 << Mat[i][j] << "\t";
	}
	output1 << std::endl;
};
output1.close();
*/
