#pragma once
#include <vector>
#include "Config.h"

#include <TApplication.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TGraph.h>
#include <TCanvas.h>

class rootplot {
public:
    rootplot(int m_window_size);
    ~rootplot();
    int window_size;

    
    bool init(int* argc, char* argv[]);
    void plt(std::vector<Point>* CH1, std::vector<Point>* CH2);
    int N;
private:
    TCanvas *c1;
    TGraph* f1;
    TGraph* f2;
    
};
