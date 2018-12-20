//Header file for picoscopeAnalysis

#include <vector>


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

