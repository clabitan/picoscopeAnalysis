#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <string.h>

#include <plotWithRoot.h>
#include <picoscopeAnalysis.h>

int main(int argc, char *argv[]) { //./picoscopeAnalysis path/to/directory run#

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
	
	std::vector <double> meanChB;
	std::vector <double> deltaT_risingEdge;
	std::vector <double> timeD_risingEdge, timeC_risingEdge;
	std::vector <std::string> eventNum;

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
				std::vector <double> time_values, chA_values, chB_values, chC_values, chD_values;  	

				//For each channel and time, get values and store in vector.
				while (getline(infile, line)) {
					double tmpTime, tmpA, tmpB, tmpC, tmpD;
					infile >> tmpTime >> tmpA >> tmpB >> tmpC >> tmpD;

					time_values.push_back(tmpTime);
					chA_values.push_back(tmpA);
					chB_values.push_back(tmpB);
					chC_values.push_back(tmpC);
					chD_values.push_back(tmpD);
				}

				//Find mean of Ch. B values
				double sumValues=0;
				for (unsigned i=0; i<chB_values.size(); i++) {
					sumValues = sumValues + chB_values[i];
				}
				int sizeVector = chB_values.size();
				meanChB.push_back(sumValues/sizeVector);	
				
				//Find trigger channel (Ch. D) rising edge	
				int timeRiseEdgeIndex = findRisingEdgeIndex(chD_values, 1500, 1);
				double timeRiseEdgeD = time_values[timeRiseEdgeIndex];
				timeD_risingEdge.push_back(timeRiseEdgeD);
	
				//Find the first 40MHz clock (Ch. C) rising edge after the trigger rising edge.
				double timeRiseEdgeC = time_values[findRisingEdgeIndex(chC_values, 0, timeRiseEdgeIndex)];
				timeC_risingEdge.push_back(timeRiseEdgeC);

				//Find the time difference between the trigger rising edge and the following 40MHz clock rising edge.
				deltaT_risingEdge.push_back(timeRiseEdgeC-timeRiseEdgeD);
			}
		}
	}
	
	double sumVal=0;
	for (unsigned i=0; i<meanChB.size(); i++) {
		sumVal = sumVal + meanChB[i];
	}	
	int sizeVec = meanChB.size();	
	double meanChB_val = sumVal/sizeVec;
	

	//Create output file for the mean Ch. B values
	std::string filename1 = "run" + run_num + "_mean_chB.txt";
	std::cout << "Creating file " << filename1 << std::endl;

	std::fstream newFile1(filename1.c_str(), std::ios::trunc | std::ios::out); //Make new file or rewrite file of same name.
	newFile1 << "Run # | Event # | Mean Voltage (V) for Ch. B \n";
	for (unsigned i=0; i<meanChB.size(); i++) {
		newFile1 << run_num << "   " << eventNum[i] << "   " << meanChB[i] << "\n";
	}
	newFile1 << run_num << "   " << "AllEvents" << "   " << meanChB_val;

	///Create output file for the time difference between the trigger (Ch D) rising edge and the 40MHz clock (Ch C) rising edge	
	std::string filename2 = "run" + run_num + "_deltaT.txt";
	std::cout << "Creating file " << filename2 << std::endl;

	std::fstream newFile2(filename2.c_str(), std::ios::trunc | std::ios::out); //Make new file or rewrite file of same name. 
	newFile2 << "Run # | Event # | Delta T (s) Between Trigger (Ch D) rising edge and 40MHz Clock (Ch C) rising edge \n"; 
	for (unsigned i=0; i<deltaT_risingEdge.size(); i++) {
		newFile2 << run_num << "   " << eventNum[i] << "   " <<  deltaT_risingEdge[i] << "\n" ;	
	}	


	///Create output file for the time corresponding to the trigger rising edge at threshold
	std::string filename3 = "run" + run_num + "_risingEdgeTime_chD.txt";
	std::cout << "Creating file " << filename3 << std::endl;

	std::fstream newFile3(filename3.c_str(), std::ios::trunc | std::ios::out); //Make new file or rewrite file of same name. 
	newFile3 << "Rising Edge Time Point for Ch D (Trigger Channel) \n"; 
	for (unsigned i=0; i<timeD_risingEdge.size(); i++) {
		newFile3 << eventNum[i] << "	" << timeD_risingEdge[i] << "\n" ;	

	}	

	///Create output file for the time corresponding to Ch C rising edge at threshold
	std::string filename4 = "run" + run_num + "_risingEdgeTime_chC.txt";
	std::cout << "Creating file " << filename4 << std::endl;

	std::fstream newFile4(filename4.c_str(), std::ios::trunc | std::ios::out); //Make new file or rewrite file of same name. 
	newFile4 << "Rising Edge Time Point for Ch C \n"; 
	for (unsigned i=0; i<timeC_risingEdge.size(); i++) {
		newFile4 << eventNum[i] << "	" << timeC_risingEdge[i] << "\n" ;	

	}	

	return 0;
} 
