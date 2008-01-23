// (Special "header" just for doxygen)

/*! @mainpage  package calibGenACD

@author Eric Charles

@section intro Introduction

This package contains code to produce calibration constants for ACD. The package includes the following executables:

<ul>
<li> runMuonCalib_Svac.exe:  Does Gain (aka MIP peak) calibrations.</li>
<li> runPedestal.exe:  Does pedestal calibrations.</li>
<li> runVetoCalib.exe:  Does veto threshold calibrations.</li>
<li> runCnoCalib.exe:  Does cno threshold calibrations.</li>
<li> runStripChart.exe:  Does time series strip chart testing</li>
<li> runCoherentNoise.exe:  Does coherent noise calibrations.</li>
<li> runRangeCalib.exe:  Does range crossover calibrations.</li>
</ul>


@section clOpts Command Line Options

@verbatim
  Usage:
    application  -c <configFile>
  
    <configFile>      : name of xml file with job configuration
 
  
    application [options] [input] -o <output>
 
      INPUT
         -r <reconFiles>   : comma seperated list of recon ROOT files
         -d <digiFiles>    : comma seperated list of digi ROOT files
         -S <svacFiles>    : comma seperated list of svac ROOT files
         -m <meritFiles>   : comma seperated list of merit ROOT files
      NOTE:  Different calibrations jobs take diffenent types of input files
 
         -o <output>       : prefix (path or filename) to add to output files
      
      OPTIONS for all jobs
         -h                : print this message
         -n <nEvents>      : run over <nEvents>
         -s <startEvent>   : start with event <startEvent>
         -I (LAT | CU06)   : specify instument being calibrated [LAT]
 
      OPTIONS for specific jobs (will be ignored by other jobs)
         -P                : use only periodic triggers
         -L                : correct for pathlength in tile
         -b <bins>         : number of time bins in strip chart [300]
         -p <pedFile>      : use pedestals from this file
         -g <gainFile>     : use gains from this file
         -R <rangeFile>    : use range data from this file
@endverbatim

@section jobOpt Example of the option file

@verbinclude src/calibOption.xml

*/
