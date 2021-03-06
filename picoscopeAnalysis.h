//Header file for picoscopeAnalysis

#include <vector>
#include <string.h>
#include <time.h> 



//Conver the timestamp to Month/Day/Year_Hour:Minute:Second:Millisecond

std::string convertTimestamp(long eventTimestamp) {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	long timeSec = eventTimestamp/1000;
	long timeMS = eventTimestamp%1000;

	rawtime = timeSec;
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%m-%d-%Y_%H:%M:%S", timeinfo);

	std::string realTime;
	realTime.append(buffer);
	realTime.append(":");
	realTime.append(std::to_string(timeMS));

	return realTime;
}



//Find the maximum value

double findMaxValue (std::vector <double> volt_values) {
	double maxVolt = -10000;
	for (unsigned i=0; i<volt_values.size(); i++) {
		if (volt_values[i]>maxVolt) maxVolt = volt_values[i];
	}
	
	return maxVolt;
}

//Find the nearest rising edge, and then find the index corresponding to the value closest to the threshold
unsigned findRisingEdgeIndex (std::vector <double> volt_values, double threshold, unsigned startingIndex) {

	if (startingIndex < 1 || startingIndex > volt_values.size() ) startingIndex = 1;
	
	double maxVolt = -10000;
	unsigned maxIndex = -10000;
	for (unsigned i=startingIndex; i<volt_values.size(); i++) {
		if (volt_values[i] > maxVolt && volt_values[i] > threshold) { //if the value is bigger than the current maximum value and over the threshold, make it the maximum value
			maxVolt = volt_values[i];
			maxIndex = i;
		}
		if (maxVolt > 0 && volt_values[i+1] < maxVolt ) break; // if you found a maximum amplitude and the next value is less than it, break

	}

	unsigned foundIndex = -10000;
	for (unsigned i=maxIndex; i>0; i--) { //starting from the maximum value, look at the values on the rising edge, and choose the one closest to the threshold value.
		if (volt_values[i] == threshold) {
			foundIndex = i;
			break;		
		}	
		else if ( volt_values[i+1] > threshold && volt_values[i-1] < threshold) {
			foundIndex = i;
			break;
		} 
	}

	while ( foundIndex < startingIndex) { //if the rising edge is before your starting index, try again
		foundIndex = findRisingEdgeIndex(volt_values, threshold, maxIndex+1);
	}

	return foundIndex;
}

