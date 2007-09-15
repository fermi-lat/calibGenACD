#ifndef AcdCalibEnum_h
#define AcdCalibEnum_h 

namespace AcdCalib {

  // Types of histograms to be fit
  enum HISTTYPE{H_NONE=-1,
		H_RAW=0,
		H_GAIN=1,
		H_VETO=2,
		H_RANGE=3,
		H_COHERENT_NOISE=5,
		H_TIME_PHA=6,		
		H_UNPAIRED=7,		
		H_FRAC=10,
		H_TIME_HIT=12,
		H_TIME_VETO=13,
		H_NHIST=14};

  // Types of calibrations
  enum CALTYPE{NONE=-1,
	       PEDESTAL=0, 
	       GAIN=1, 
	       VETO=2,
	       RANGE=3,
	       CNO=4,	       
	       COHERENT_NOISE=5,
	       TIME_PROF=6,
	       UNPAIRED=7, 
	       HITMAP=8,
	       MERITCALIB=9,
	       NDESC=10};  

  // Types of input data chains
  enum CHAIN{NO_CHAIN=-1,
	     DIGI=0,
	     RECON=1,
	     MERIT=2,
	     SVAC=3,
	     BENCH=4,
	     NCHAIN=5};

  // Types of strip chart normalization
  enum STRIPTYPE{ PLAIN,
		  MEAN_ABSOLUTE,
		  MEAN_RELATIVE,
		  MEAN_SIGMA };

};

#endif
