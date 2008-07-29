#ifndef AcdCalibLoop_Digi_h
#define AcdCalibLoop_Digi_h 

// base class
#include "AcdCalibBase.h"

// local includes
#include "./AcdGarcGafeHits.h"

// stl includes
#include <iostream>

// forward declares
class TH2;
class TChain;
class DigiEvent;

/** 
 * @class AcdCalibLoop_Digi
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

class AcdCalibLoop_Digi : public AcdCalibBase {

public :
  
  /// Standard ctor, where user provides the input data
  AcdCalibLoop_Digi(AcdCalibData::CALTYPE t, TChain *digiChain, Bool_t requirePeriodic = kFALSE, AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdCalibLoop_Digi();  

  /// Divide one set of histograms by another
  AcdHistCalibMap* makeRatioPlots();

protected:
  
  /// setup input data
  Bool_t attachChains();

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId, Double_t& timeStamp);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);

  /// Called each event to do mapping from TILE:PMT to GARC:GAFE space
  void fillCnoData();

private:

  /// pointer to a DigiEvent
  DigiEvent* m_digiEvent;
  
  /// Take only periodic events
  Bool_t m_requirePeriodic;

  // Various histograms to fill
  AcdHistCalibMap* m_rawHists;
  AcdHistCalibMap* m_vetoHists;
  AcdHistCalibMap* m_rangeHists;
  
  /// Input pedestal calibration
  AcdCalibMap* m_peds;
  /// Input range calibration
  AcdCalibMap* m_ranges;

  /// Used to work in GARC:GAFE space instead of TILE:PMT space
  AcdGarcGafeHits m_garcGafeHits;  
    
};

#endif
