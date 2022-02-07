#include"rootplot.h"

// Initalize ROOT
TApplication rootapp("spectrum", 0, 0);

rootplot::rootplot(int m_window_size) {
	window_size = m_window_size;
	c1 = new TCanvas("c1", "Spectrum analyzer in ROOT");
	f1 = new TGraph(window_size);
	f2 = new TGraph(window_size);
	
}

rootplot::~rootplot() {

}

bool rootplot::init(int* argc, char* argv[]) {

	// Reate the canvas we are drawing on
	
	c1->SetWindowSize(1550, 700);

	// Create the spectrogram
	
	f1->SetTitle("Power Density Spectrum");
	f1->GetXaxis()->SetTitle("Frequency (MHz)");

	f1->GetYaxis()->SetTitle("dB (V^2/Hz)");
	f1->GetYaxis()->SetTitleSize(0.05);
	f1->GetYaxis()->SetTitleOffset(0.5);

	f1->SetMinimum(-120);
	f1->SetMaximum(-10);

	// Creathe the waveform plot
	
	f2->SetTitle("Waveform");
	f2->GetXaxis()->SetTitle("time (ns)");
	f2->GetYaxis()->SetTitle("Amplitude (V)");
	f2->GetYaxis()->SetTitleSize(0.05);
	f2->GetYaxis()->SetTitleOffset(0.5);
	f2->SetMinimum(-1);
	f2->SetMaximum(1);

	// divide the canvas into two vertical sub-canvas
	c1->Divide(1, 2);

	// "Register" the plots for each canvas slot
	c1->cd(1); // Set current canvas to canvas 1 (yes, 1 based indexing)
	c1->Pad()->SetLeftMargin(0.06);
	c1->Pad()->SetRightMargin(0.01);
	f1->Draw();
	c1->cd(2); // Set current canvas to canvas 2
	c1->Pad()->SetLeftMargin(0.06);
	c1->Pad()->SetRightMargin(0.01);
	f2->Draw();
	return true;

}

void rootplot::plt(std::vector<Point>* FFT, std::vector<Point>* waveform) {
	double f, dB;	
	int trigger_index = 0;
	float pre_adc_value = waveform->data()[0].y;;	
	//trigger setting
	for (int i = 0; i < waveform->size(); i++) {
		if ((pre_adc_value - waveform->data()[i].y) < 0 && waveform->data()[i].y > 0.05 && waveform->data()[i].y < 0.3) {
			trigger_index = i;
			break;
		}
		pre_adc_value = waveform->data()[i].y;
		
	}
	

	// Update the spectrogram & waveform
	for (size_t i = 0; i < window_size; i++) {
		//spectrum
		f = FFT->data()[i].x / 1000;
		dB = 10 * log10(FFT->data()[i].y);
		f1->SetPoint(i, f, dB);

		//waveform
		
		f2->SetPoint(i, i, waveform->data()[trigger_index+i].y);
	}
	
	//Notify ROOT that the plots have been modified and needs update
	c1->cd(1);
	c1->Update();
	//c1->Pad()->Draw();
	c1->cd(2);
	c1->Update();
	//c1->Pad()->Draw();

	//gSystem->ProcessEvents();
	
}