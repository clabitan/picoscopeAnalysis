#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <string.h>

#include <TStyle.h>
#include <RD53Style.C>
#include <plotWithRoot.h>
#include <picoscopeAnalysis.h>
#include <TGaxis.h>

int main(int argc, char *argv[]) { //./picoscopeAnalysis path/to/directory run#

	SetRD53Style();	
	gStyle->SetTickLength(0.02);
	gStyle->SetTextFont();
	
	if (argc < 3) {
		std::cout << "No directory or run# given! \nExample: ./picoscopeAnalysis path/to/directory/ run# \nExample: ./plotWithRoot_Picoscope /misc/archive/2018_11-05-slac_tb/yarr_data/scope/run_1541558824/ 511" << std::endl;
		return -1;
	}

	std::string dir, filepath, file_name, run_num, event_num;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;

	dp = opendir(argv[1]);	//open directory
	if (dp==NULL) {	//if directory doesn't exist
		std::cout << "Directory not found. " << std::endl;
		return -1;
	}

	dir = argv[1];
	run_num = argv[2];

	std::vector<double> meanChB;
	std::vector <double> deltaT_risingEdge;
	std::vector <double> timeD_risingEdge, timeC_risingEdge;
	std::vector <std::string> eventNum;

//	filename << "plots_run" << run_num << ".root";  
//	TFile graphFile(filename.c_str(), "RECREATE"); //Create root file, or rewrite file of the same name.


	while ((dirp = readdir(dp))) { //pointer to structure representing directory entry at current position in directory stream, and positions directory stream at the next entry. Returns a null pointer at the end of the directory stream.

		file_name = dirp->d_name;
		filepath = dir + "/" + dirp->d_name;
		const char *file_path = filepath.c_str();		

		if (stat(filepath.c_str(), &filestat)) continue; //skip if file is invalid
		if (S_ISDIR(filestat.st_mode)) continue; //skip if file is a directory

		if ( strstr( file_path, "frame_") != NULL && strstr(file_path, ".csv") != NULL ) { //if filename contains string declared in argument.
			//std::cout << "Opening file: " << filepath.c_str() << std::endl;
			std::string filename = filepath.c_str();
			std::fstream infile(filepath.c_str(), std::ios::in);

			event_num = file_name.substr(file_name.find("_")+1, 13);
			eventNum.push_back(event_num);

			std::string line;	

			std::getline(infile, line); //skip Header

			if (!infile) {
				std::cout << "Something wrong with file ..." << std::endl;
				//return -1;
			}

			else {	
				std::vector <double> time_values, ch_values[4], chC_multipliedShifted;	

				//For each channel and time, get values and store in vector.
				while (getline(infile, line)) {
					double tmpTime, tmpA, tmpB, tmpC, tmpD;
					infile >> tmpTime >> tmpA >> tmpB >> tmpC >> tmpD;

					time_values.push_back(tmpTime*(1e6));
					ch_values[0].push_back(tmpA);
					ch_values[1].push_back(tmpB);
					ch_values[2].push_back(tmpC);
					chC_multipliedShifted.push_back((tmpC*10)+1000);
					ch_values[3].push_back(tmpD);
				}

				//Find mean of Ch. B values
				double sumValues=0;
				for (unsigned i=0; i<ch_values[1].size(); i++) {
					sumValues = sumValues + ch_values[1][i];
				}
				int sizeVector = ch_values[1].size();
				meanChB.push_back(sumValues/sizeVector);		
				
				//Find trigger channel (Ch. D) rising edge
				int timeRiseEdgeIndex = findRisingEdgeIndex(ch_values[3], 1500, 1);
				double timeRiseEdgeD = time_values[timeRiseEdgeIndex];
				timeD_risingEdge.push_back(timeRiseEdgeD);
	
				//Find the first 40MHz clock (Ch. C) rising edge after the trigger rising edge.
				double timeRiseEdgeC = time_values[findRisingEdgeIndex(ch_values[2], 0, timeRiseEdgeIndex)];
				timeC_risingEdge.push_back(timeRiseEdgeC);	
				deltaT_risingEdge.push_back(timeRiseEdgeC-timeRiseEdgeD);

				std::string graphName = event_num + "plot" + ".png";				

				//Create RD53A Internal Label
				TLatex *tname = new TLatex();
				tname->SetNDC();
				tname->SetTextAlign(22);
				tname->SetTextFont(73);
				tname->SetTextSizePixels(60);
				std::string rd53 = "RD53A Internal";

				//Create TCanvas for graphs
				TCanvas *canvas = new TCanvas("canvas", "canvas", 0, 0, 1600, 1200);
				style_TH1canvas(canvas);
				canvas->SetRightMargin(0.09);

				//Create TGraphs for each channel
				std::string graphNames[4] = {"plotA", "plotB", "plotC", "plotD"};
				std::string chNames[4] = {"A", "B", "C", "D"};
				int color[4] = {4, 2, 418, 800}; // {Blue, Red, Green, Yellow) to match the color scheme of the picoscope	
				const int n = time_values.size();
				TGraph *graphs[4];
			//	TGraph *graph = new TGraph(n, &time_values[0], &ch_values[3][0]);
			//	graph->Draw("LP A");	

				for (int i=0; i<4; i++) {
					if (i !=2) graphs[i] = new TGraph(n, &time_values[0], &ch_values[i][0]);
					else if (i == 2) {graphs[i] = new TGraph(n, &time_values[0], &chC_multipliedShifted[0]); }
					style_TGraph(graphs[i], "Time [#mus]", "Voltage [V]");
					graphs[i]->GetYaxis()->SetTitleSize(0.05);
					graphs[i]->GetYaxis()->SetTitleOffset(1.43);
					graphs[i]->GetYaxis()->SetLabelSize(0.05);
					graphs[i]->SetLineColor(color[i]);
					graphs[i]->SetLineWidth(3);
					graphs[i]->SetMarkerColor(color[i]);
					graphs[i]->SetMarkerStyle(20);
					graphs[i]->SetMarkerSize(2);
					graphs[i]->SetName(chNames[i].c_str());
					graphs[i]->SetTitle(event_num.c_str());
				} 
				
				//Draw Trigger channel first
				graphs[3]->Draw("LP A");
				for (int i=0; i<3; i++) { graphs[i]->Draw("LP SAME"); } //maybe try TMultigraph???

				//Since TGraph's aren't defined with a name, need to use SetName first before adding to the legend
				//Create TLegend
				TLegend *lb = new TLegend(0.78, 0.70, 0.84, 0.91);
				lb->SetHeader("Channels", "C");
				for (int i=0; i<4; i++) { lb->AddEntry(chNames[i].c_str(), chNames[i].c_str(), "lp"); }
				lb->SetBorderSize(0);
				lb->Draw();	
				
				//Write RD53A Internal
				tname->DrawLatex(0.28,0.96,rd53.c_str());
				tname->DrawLatex(0.78, 0.96, event_num.c_str());
		
				//Draw a vertical line at the trigger channel threshold
				TLine *trigLine = new TLine(timeRiseEdgeD, 0, timeRiseEdgeD, 3100);
				trigLine->SetLineColor(1);
				trigLine->SetLineWidth(2);
				trigLine->SetLineStyle(10);
				trigLine->Draw();

				//Draw a vertical line at the 40MHz channel threshold
				TLine *clockLine = new TLine(timeRiseEdgeC, 0, timeRiseEdgeC, 3100);
				clockLine->SetLineColor(6);
				clockLine->SetLineWidth(2);
				clockLine->SetLineStyle(10);
				clockLine->Draw();

				//Draw a horizontal line for the time difference
				TLine *diffLine = new TLine(timeRiseEdgeD, 2000, timeRiseEdgeC, 2000);
				diffLine->SetLineColor(1);
				diffLine->SetLineWidth(4);
				diffLine->SetLineStyle(1);
				diffLine->Draw();

				char deltaT[100] = {};	
				sprintf(deltaT, "#DeltaT = %.2fns", (timeRiseEdgeC-timeRiseEdgeD)*1e3);
				tname->DrawLatex(0.78, 0.68, deltaT);	
				
				graphs[3]->GetXaxis()->SetRangeUser(0.79, 0.83);
				graphs[3]->GetYaxis()->SetRangeUser(0,3100);
				canvas->Update();				

				double maxCvalue = findMaxValue(chC_multipliedShifted);
				
				//Draw a new pad for the second y-axis
				TPad *pad2 = new TPad("pad2", "", 0,0,1,1);
				pad2->SetFillStyle(0);
				pad2->SetFillColor(0);
				pad2->SetFrameFillStyle(0);
				pad2->SetBorderSize(0);
				pad2->SetFrameLineWidth(0);
				pad2->SetFrameBorderMode(0);
				pad2->Draw();
				pad2->cd();

				//Draw a y-axis for Ch C 
				TGaxis *axisC = new TGaxis(0.91, 0.12, 0.91, 0.93, -100, 210, 506, "+L"); //pad x/y min=0 and max=1
				axisC->SetName("axisC");
				axisC->SetLabelColor(418);
				axisC->SetLineColor(418);
				axisC->SetTitle("Channel C Voltage [V]");
				axisC->SetTitleColor(418);
				axisC->SetTitleOffset(1.1);
				axisC->Draw();			
	
				canvas->SaveAs(graphName.c_str());
			//	c->Print(graphName.c_str());
			
				delete canvas;
//				delete graph;
//				for (int i=0; i<4; i++) {delete graphs[i];}
			}
		}
	}

	double sumVal=0;
	for (unsigned i=0; i<meanChB.size(); i++) {
		sumVal = sumVal + meanChB[i];
	}	
	int sizeVec = meanChB.size();	
	double meanChB_val = sumVal/sizeVec;
	

	std::string filename1 = "run" + run_num + "_mean_chB.txt";
	std::cout << "Creating file " << filename1 << std::endl;

	//Create output file for the mean Ch. B values
	std::fstream newFile1(filename1.c_str(), std::ios::trunc | std::ios::out); //Make new file or rewrite file of same name.
	newFile1 << "Run # | Event # | Mean Voltage (V) for Ch. B \n";
	for (unsigned i=0; i<meanChB.size(); i++) {
		newFile1 << run_num << "   " << eventNum[i] << "   " << meanChB[i] << "\n";
	}
	newFile1 << run_num << "   " << "AllEvents" << "   " << meanChB_val;

	std::string filename2 = "run" + run_num + "_deltaT.txt";
	std::cout << "Creating file " << filename2 << std::endl;

	///Create output file for the time difference between the trigger (Ch D) rising edge and the 40MHz clock (Ch C) rising edge	
	std::fstream newFile2(filename2.c_str(), std::ios::trunc | std::ios::out); //Make new file or rewrite file of same name. 
	newFile2 << "Run # | Event # | Delta T (s) Between Trigger (Ch D) rising edge and 40MHz Clock (Ch C) rising edge \n"; 
	for (unsigned i=0; i<deltaT_risingEdge.size(); i++) {
		newFile2 << run_num << "   " << eventNum[i] << "   " <<  deltaT_risingEdge[i] << "\n" ;	
	}	


	std::string filename3 = "run" + run_num + "_risingEdgeTime_chD.txt";
	std::cout << "Creating file " << filename3 << std::endl;

	///Create output file for the time corresponding to the trigger rising edge at threshold
	std::fstream newFile3(filename3.c_str(), std::ios::trunc | std::ios::out); //Make new file or rewrite file of same name. 
	newFile3 << "Rising Edge Time Point for Ch D (Trigger Channel) \n"; 
	for (unsigned i=0; i<timeD_risingEdge.size(); i++) {
		newFile3 << eventNum[i] << "	" << timeD_risingEdge[i] << "\n" ;	

	}	

	std::string filename4 = "run" + run_num + "_risingEdgeTime_chC.txt";
	std::cout << "Creating file " << filename4 << std::endl;

	///Create output file for the time corresponding to Ch C rising edge at threshold
	std::fstream newFile4(filename4.c_str(), std::ios::trunc | std::ios::out); //Make new file or rewrite file of same name. 
	newFile4 << "Rising Edge Time Point for Ch C \n"; 
	for (unsigned i=0; i<timeC_risingEdge.size(); i++) {
		newFile4 << eventNum[i] << "	" << timeC_risingEdge[i] << "\n" ;	

	}	

	return 0;
} 
