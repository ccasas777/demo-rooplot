// root_exercise.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <time.h>
#include <complex.h>
#include "fftw3.h"
#include <fstream>
#include <sstream>

#include "rootplot.h"

#define M_PI 3.14159
using namespace std;

rootplot* m_rootplot;
fftw_complex* out;
fftw_plan p;
double* in;
double* window_data;
#pragma comment(lib, "ws2_32.lib")

void SetDFT(int nbrOfInputSamples) {
	int nbrOfOutputSamples = ceil(nbrOfInputSamples / 2.0);
	in = (double*)fftw_malloc(sizeof(double) * nbrOfInputSamples);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * nbrOfOutputSamples);
	p = fftw_plan_dft_r2c_1d(nbrOfInputSamples, in, out, FFTW_ESTIMATE);
	window_data = (double*)malloc(sizeof(double) * nbrOfInputSamples);
}

#define HAMMING 0
#define HANNING 1
#define BLACKMAN_HARRIS 2
void SetWindow(int window_mode, double* window, int InputSamples)
{
	for (int i = 0; i < InputSamples; i++) {
		switch (window_mode)
		{
		case 0:
			// Hamming
			window[i] = (0.54 - 0.46 * cos(2 * M_PI * i / (InputSamples - 1)));
		case 1:
			// Hanning
			window[i] = (0.5 * (1 - cos(2 * M_PI * i / (InputSamples - 1))));
		case 2:
			// Blackman-Harris
			window[i] = 0.35875 - 0.48829 * cos(2 * M_PI * i / (InputSamples)) + 0.14128 * cos(4 * M_PI * i / (InputSamples)) - 0.01168 * cos(6 * M_PI * i / (InputSamples));;
		}
	}
}

void DisableDFT() {
	fftw_destroy_plan(p);
	fftw_free(in); fftw_free(out);
}

int save_trigger = 0;
vector<Point> performDFT(vector<Point>* input_data, float MIN_FREQ, float MAX_FREQ)
{

	float sampleRate = 1e9; //1 GHz after interpolation
	int nbrOfInputSamples = input_data->size();
	int nbrOfOutputSamples = ceil(nbrOfInputSamples / 2.0);

	// Read data from input file to input array
	double value;
	int     idx = 0;
	while (idx < nbrOfInputSamples) {
		in[idx++] = window_data[idx++] * input_data->data()[idx++].y;
	}

	// Perform the dft
	fftw_execute(p);


	double  realVal;
	double  imagVal;
	double  powVal;
	double  absVal;
	double  df = sampleRate / nbrOfOutputSamples / 2000; //unit: kHz

	double  f;
	vector<Point> FFTW_data;

	for (idx = 0; idx < nbrOfOutputSamples - 1; idx++) {
		Point tmp_data;
		f = idx * df;
		if (f > MIN_FREQ && f < MAX_FREQ) {
			realVal = out[idx][0] / nbrOfInputSamples; // Ideed nbrOfInputSamples is correct!
			imagVal = out[idx][1] / nbrOfInputSamples; // Ideed nbrOfInputSamples is correct!
			powVal = 2 * (realVal * realVal + imagVal * imagVal) + +0.000000001 * rand() / (RAND_MAX + 1.0);
			tmp_data.x = f;
			tmp_data.y = powVal;
			FFTW_data.push_back(tmp_data);
			if (idx == 0) {
				powVal /= 2;
			}

		}

	}

	return FFTW_data;

}



int main(int argc, char* argv[])
{
	int window_size = 2048; //sample length of display


	//initialize rootplot core
	m_rootplot = new rootplot(window_size);
	m_rootplot->init(&argc, argv);



	// Read DATA

	ifstream infile("../data/T40M_250MSPS_INTERPOL.csv", ios::in);
	string line;
	vector<struct Point> DATA_40M;
	getline(infile, line);
	while (getline(infile, line)) {
		stringstream ss(line);
		string str;
		Point adc_data;

		getline(ss, str, ',');
		adc_data.x = stod(str) * 1; //unit: ns
		getline(ss, str, ',');
		adc_data.y = stold(str); //unit V

		DATA_40M.push_back(adc_data);
	}

	//prepare DFT Data
	int size = DATA_40M.size();	
	SetDFT(size);
	SetWindow(HANNING, window_data, size);
	std::vector<Point> FFT_Data;
	int min_freq = 1e3;//unit: kHz
	int max_freq = 8e4;
	FFT_Data = performDFT(&DATA_40M, min_freq, max_freq);

	if (FFT_Data.size() < window_size || size < window_size) {
		printf("error1\r\n");
		return 0;
	}
	

	//performance test
	double tt_start = clock();
	int test_counter = 0;
	int N = 100;
	while (test_counter < N) {
		
		m_rootplot->plt(&FFT_Data, &DATA_40M);
		test_counter++;
	}
	
	double tt_end = clock();
	double duration = (tt_end - tt_start);
	printf("draw_time(%d frames) = %.2f ms\r\n",N, duration);
	system("pause");
	
}
