[//]: # (Image References)
[image1]: ./snapshot/result.JPG

# Rootplot-Demo-Project


### Goals
Comparing to the matplotlib.h, the root lib. is another good choice if you want to numerical visualize the data in C++ program.
Here is a simple demo that shows a waveform and a spectrum together. 
In last, for a purpose of testing the plotting speed of the root core, we run the same data N times to calculate the time.

 
## File Overview  

1. root_exercise.cpp : initialize the Classes and modules.

2. fftw3.h & libfftw3-3.dll & libfftw3-3.lib : It's a high efficiency DFT program. if you like to know details, please refer to https://www.fftw.org/ 

3. rootplot.h & rootplot.cpp : It's a simple class to integrate the root core functions for 2D numerical plot.

## Environmental setup

1. install VS2019 for Cpp coding.

2. install root from the official download website. downlading the *.exe release version for windows base.

ps: my version is "root_v6.24.06". my install dir: C:\root_v6.24.06

3. add windows environmental parameters as following

PATH: C:\root_v6.24.06\bin

4. open the CPP project property, adding the include and lib address:

Additional include Directories:  C:\root_v6.24.06\include

Additional Library Directories: C:\root_v6.24.06\lib; <PRJ_DIR>/lib/

Add dependencies: C:\root_v6.24.06\lib\*.lib; libfftw3-3.lib


## Descriptions

To begining with, we first generate the data from .csv and initialize the root core.

```cpp

 int window_size = 2048; //sample length of display


 //initialize rootplot core
 m_rootplot = new rootplot(window_size);
 m_rootplot->init(&argc, argv);


 ifstream infile("../data/T40M_250MSPS.csv", ios::in);
    string line;
    vector<struct Point> DATA_40M;
    getline(infile, line);
    while (getline(infile, line)) {
        stringstream ss(line);
        string str;
        Point adc_data;

        getline(ss, str, ',');
        adc_data.x = stod(str) * 4; //unit: ns
        getline(ss, str, ',');
        adc_data.y = stold(str); //unit V

        DATA_40M.push_back(adc_data);

    }


```

Here is to prepare the DFT data to plot later by FFTW3 API

```cpp
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

```


Finally, I use rootplot to visualize the data and test the plotting speed.

```cpp

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


```
## Result:　
N=100, the cost time = ~2000 ms (matplotlib needs around 4000-5000 ms)
![image1]






