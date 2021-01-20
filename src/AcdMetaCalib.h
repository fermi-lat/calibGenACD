#ifndef AcdMetaCalib_h
#define AcdMetaCalib_h 

// base class
#include "AcdCalibBase.h"

// stl includes
#include <iostream>
#include <list>
#include <map>

// forward declares
class AcdVetoConfig;
class AcdHldConfig;

/** 
 * @class AcdMetaCalib
 *
 * @brief AcdCalibration class that merges input calibration to make other calibrations
 *
 * This is used for the following calibrations:
 *   - Pedestals
 *   - Veto thresholds
 *   - CNO thresholds
 *   - Range crossover
 *   - Gains without pathlength correction
 *   - Looking at unpaired hits (only 1 PMT fired in a detector element)
 *   - Comparing GEM data to ACD hitmap
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdMetaCalib.h,v 1.3 2008/08/06 17:46:09 echarles Exp $
 */

class AcdMetaCalib : public AcdCalibBase {

public :
  
  /// Standard ctor, where user provides the input data
  AcdMetaCalib(AcdCalibData::CALTYPE t, AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdMetaCalib();  

  /// Read in a settings/ set point pair
  Bool_t readSettingsSetPointPair(const std::string& settingsFile,
				  const std::string& setPointFile);

  /// Fill the veto set point v. dac settings histograms
  Bool_t fillVetoThresholds();

  /// Fill the cno set point v. dac settings histograms
  Bool_t fillCnoThresholds();

  /// Fit the high range calibration histograms
  Bool_t fillHighRangeData();

  /// Copy the charge scale values from the input calibration to the output calibration
  Bool_t copyPeakVals();

protected:
  
  /// read in 1 event
  virtual Bool_t readEvent( ) { return kFALSE; }

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used) { used = kFALSE; }
  

private:

  /// Histograms of set points v. dac_settings
  AcdHistCalibMap* m_fitHists;  
  /// Histograms of high range calibrations
  AcdHistCalibMap* m_highRangeHists;

  /// Pairs of input files for veto calibration
  std::list< std::pair< AcdVetoConfig*, AcdCalibMap* > > m_vetoPoints;
  /// Pairs of input files for cno calibration
  std::list< std::pair< AcdHldConfig*, AcdCalibMap* > >  m_cnoPoints;
      
};

#endif
