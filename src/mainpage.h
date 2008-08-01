// (Special "header" just for doxygen)

/*! @mainpage  package calibGenACD

@author Eric Charles

@section intro Introduction

This package contains code to produce calibration constants for ACD. The package includes the following executables:

<ul>
  <li>runPedestal.exe: runs pedestal calibrations using periodic triggers</li>
  <li>runMipCalib.exe: runs mip calibrations using charged particles and tracker data</li>
  <li>runRangeCalib.exe: runs range crossover calibration using all event data</li>
  <li>runVetoCalib.exe: runs veto threshold calibration using all event data</li>
  <li>runCnoCalib.exe: runs veto threshold calibration using all event data</li>
  <li>runCoherentNoiseCalib.exe: runs coherent noise calibration using periodic triggers</li>
  <li>runCarbonCalib.exe: runs carbon peak calibration using CAL GCR selection and tracker data</li>
  <li>runRibbonCalib.exe: runs ribbon attenuation calibrations using charged particles and tracker data</li>
  <li>runHighPed.exe: runs pedestal calibrations special AcdPedestal run</li>
  <li>runVetoFitCalib.exe: runs veto threshold v. setting calibration using special calibraiton runs</li>
  <li>runCnoFitCalib.exe: runs cno threshold v. setting calibration using special calibraiton runs</li>
  <li>runHighRangeCalib.exe: creates high range PHA -> MIP calibration using various inputs</li>
  <li>runMeritCalib.exe: runs a check on gains, thresholds and range calibrations</li>
  <li>calibReport.exe: make the HTML report for a calibration</li>
  <li>makeResultTree.exe: convers the calibration xml file to a root file</li>
</ul>


@section clOpts Command Line Options

@verbatim
  Usage:
    application [options] <input>
 
     INPUT file types
        *.root            : root files (including xrootd)
        *.lst, *.txt      : text files with list of root files
     
     OPTIONS for all jobs
        -h                : print this message
        -o <string>       : prefix to add to output files
        -x <fileName>     : refrence calibration, may be used to seed fits
        -n <int>          : run over <nEvents>
        -s <int>          : start with event <startEvent>
        -I (LAT | CU06)   : specify instument being calibrated [LAT]
 *
     OPTIONS for specific jobs (will be ignored by other jobs)
        -P                : use only periodic triggers
        -G <int>          : use CAL GCR selection for a given Z
        -p <fileName>     : use pedestals from this file
        -R <fileName>     : use range data from this file
        -g <fileName>     : use gains from this file
        -H <fileName>     : use high range pedestals from this file
        -C <fileName>     : use carbon peak calibration from this file
@endverbatim

*/
