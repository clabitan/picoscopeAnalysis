# picoscopeAnalysis
These scripts were intially written to analyze RD53A picoscope data for the November 2018 SLAC Test Beam Runs 506-513. They are similar to the scripts in Yarr/src/scripts and use the same header files (included in this repository). 


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

This program does the same as picoscopeAnalysis.cxx, with the addition of creating a root file containing plots for each event and creating an output file containing the time difference between each event. For the output file containing the time difference between each event, times <100ms and >300ms are marked with '!!!'; this is because events are supposed to be 200ms apart. 

This uses frame\_\*.csv files.

```bash

./picoscopeAnalysis_TGraph path/to/directory run#

```

Example plot when saved from root file:
<object data="Images/PicoscopeAnalysisPlot_RootFileExample.pdf" type="application/pdf" width="700px">
	<embed src="Images/PicoscopeAnalysisPlot_RootFileExample.pdf">
		<p> This browser does not support PDFs. Please download the PDF to view it: <a href="Images/PicoscopeAnalysisPlot_RootFileExample.pdf"> PicoscopeAnalysisPlot_RootFileExample PDF</a>.</p>
	</embed>
</object>

Example plot when enabling canvas->SaveAs():
<object data="Images/PicoscopeAnalysisPlot_SaveExample.pdf" type="application/pdf" width="700px">
        <embed src="Images/PicoscopeAnalysisPlot_SaveExample.pdf">
                <p> This browser does not support PDFs. Please download the PDF to view it: <a href="Images/PicoscopeAnalysisPlot_SaveExample.pdf"> PicoscopeAnalysisPlot_SaveExample PDF</a>.</p>
        </embed>
</object>

