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
 * @brief AcdCalibration class that loop on Digis
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
 * $Header$
 */

class AcdMetaCalib : public AcdCalibBase {

public :
  
  /// Standard ctor, where user provides the input data
  AcdMetaCalib(AcdCalibData::CALTYPE t, AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdMetaCalib();  

  /// Read in a settings/ set point pair
  Bool_t readSettingsSetPointPair(const std::string& settingsFile,
				  const std::string& setPointFile);

  Bool_t fillVetoThresholds();

  Bool_t fillCnoThresholds();

  Bool_t fillHighRangeData();

  Bool_t copyPeakVals();

protected:
  
  /// read in 1 event
  virtual Bool_t readEvent( ) { return kFALSE; }

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used) { used = kFALSE; }
  

private:

  // Various histograms to fill
  AcdHistCalibMap* m_fitHists;
  AcdHistCalibMap* m_highRangeHists;

  std::list< std::pair< AcdVetoConfig*, AcdCalibMap* > > m_vetoPoints;
  std::list< std::pair< AcdHldConfig*, AcdCalibMap* > >  m_cnoPoints;
      
};

#endif
