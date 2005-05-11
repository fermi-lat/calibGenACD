// (Special "header" just for doxygen)

/*! @mainpage  package calibGenACD

@author Xin Chen

@section intro Introduction

This package contains code to produce pedestal and gain calibration constants for ACD. The package includes the following executables:

<ul>
<li> runMuonCalib.exe: it can read in a job option xml file or use ../src/muonCalibOption.xml as default. It process a set of digi and a set of recon root files to produce the constants. The code assumes ACD data is taken with full LAT so that reconstructed direction can be used in calibration, it also requires data to be non zero suppressed in order to calibrate the pedestal. </li>
</ul>

@section jobOpt Example of the option file

@verbinclude ./muonCalibOption.xml

*/
