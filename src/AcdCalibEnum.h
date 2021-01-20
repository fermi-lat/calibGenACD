#ifndef AcdCalibEnum_h
#define AcdCalibEnum_h 

/**
 * @brief Data for ACD calibrations
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibEnum.h,v 1.10 2014/02/14 03:29:00 echarles Exp $
 **/

namespace AcdCalib {

  /// Types of histograms to be fit
  enum HISTTYPE{H_NONE=-1,
		H_RAW=0,             // Used for Pedestal and Ped_High: 1 PHA/bin
		H_GAIN=1,            // Used for MIP peak: 32 PHA /bin (0.05 MIP/bin for check)
		H_VETO=2,            // Used for Veto Set Point: Few PHA/bin (0.01 MIP/bin for check)
		H_RANGE=3,           // Used for Range Crossover: 1 PHA/bin (0.05 log10(MIP)/bin for check)
		H_CNO=4,             // Used for CNO Set Point: Few PHA/bin (0.25 MIP/bin for check)
		H_HIGH_RANGE=5,      // Used for High Range Calibration fit: 1 MIP/bin)
		H_COHERENT_NOISE=6,  // Used for Coherent Noise, 10 ticks/bin
		H_RIBBONS=7,         // Used for Ribbons: 16 PHA/bin
		H_CARBON=8,          // Used for Carbon peak: 16 PHA/bin
		H_VETO_FIT=9,        // Used for VETO and CNO settings fit: 1 PHA/bin
		H_TREND=10,          // Used for Trending: 1 calib/bin
		H_NHIST=11};

  /// Types of input data chains
  enum CHAIN{NO_CHAIN=-1,
	     DIGI=0,
	     SVAC=1,
	     MERIT=2,
	     RECON=3,
	     OVERLAY=4,	    
	     NCHAIN=5};

};

#endif
