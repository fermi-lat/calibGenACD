#ifndef AcdCalibEnum_h
#define AcdCalibEnum_h 

/**
 * @brief Data for ACD calibrations
 *
 * @author Eric Charles
 * $Header$
 **/

namespace AcdCalib {

  /// Types of histograms to be fit
  enum HISTTYPE{H_NONE=-1,
		H_RAW=0,
		H_GAIN=1,  // also used for CARBON
		H_VETO=2,  // also use for CNO
		H_RANGE=3,		
		H_HIGH_RANGE=4,
		H_COHERENT_NOISE=5,
		H_RIBBONS=6,
		H_VETO_FIT=7, // also used for CNO_FIT
		H_TIME_PHA=8,	
		H_TIME_HIT=9,
		H_TIME_VETO=10,
		H_UNPAIRED=11,		
		H_NHIST=12};

  /// Types of input data chains
  enum CHAIN{NO_CHAIN=-1,
	     DIGI=0,
	     RECON=1,
	     MERIT=2,
	     SVAC=3,
	     BENCH=4,
	     NCHAIN=5};

  /// Types of strip chart normalization
  enum STRIPTYPE{ PLAIN,
		  MEAN_ABSOLUTE,
		  MEAN_RELATIVE,
		  MEAN_SIGMA };

};

#endif
