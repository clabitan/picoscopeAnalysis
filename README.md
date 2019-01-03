# picoscopeAnalysis
These scripts were intially written to analyze RD53A picoscope data for the November 2018 SLAC Test Beam Runs 506-513. They are similar to the scripts in Yarr/src/scripts and use the same header files. 


## Compile with Makefile

To compile, do 'make'.

```bash
$ make
[Compiling] picoscopeAnalysis.o
[Linking] picoscopeAnalysis
[Compiling] picoscopeAnalysis_TGraph.o
[Linking] picoscopeAnalysis_TGraph
```

## Running Scripts

### picoscopeAnalysis.cxx

This program calculates the time difference between the trigger (Ch. D) rising edge and the first 40MHz clock (Ch. C) rising edge after the trigger. The values are then saved in an output file (run5XX\_deltaT.txt).

Debugging version: Currently also creates files for the rising edge time of both the trigger channel (Ch. D) and the 40MHz clock (Ch. C). 


This uses frame\_\*.csv files.

```bash

./picoscopeAnalysis path/to/directory run#

```


### picoscopeAnalysis_TGraph.cxx

This program does the same as picoscopeAnalysis.cxx, with the addition of creating a root file containing plots for each event.


<object data="Images/PicoscopeAnalysisPlot_RootFileExample.pdf" type="application/pdf" width="700px">
	<embed src="Images/PicoscopeAnalysisPlot_RootFileExample.pdf">
		<p> This browser does not support PDFs. Please download the PDF to view it: <a href="Images/PicoscopeAnalysisPlot_RootFileExample.pdf"> Download PDF</a>.</p>
	</embed>
</object>

