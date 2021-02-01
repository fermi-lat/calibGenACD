#ifndef AcdCalibLoop_OVL_h
#define AcdCalibLoop_OVL_h 

// base class
#include "AcdCalibBase.h"

// local includes
#include "./AcdGarcGafeHits.h"

// stl includes
#include <iostream>

// forward declares
class TH2;
class TChain;
class EventOverlay;

/** 
 * @class AcdCalibLoop_OVL
 *
 * @brief AcdCalibration class that loop on Overlays
 *
 * This is used for the following calibrations:
 *   - Pedestals (low and high range)
 *   - Veto thresholds
 *   - CNO thresholds
 *   - Range crossover
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibLoop_OVL.h,v 1.1 2014/02/14 03:29:00 echarles Exp $
 */

class AcdCalibLoop_OVL : public AcdCalibBase {

public :
  
  /// Standard ctor, where user provides the input data
  AcdCalibLoop_OVL(AcdCalibData::CALTYPE t, TChain *ovlChain, 
		   AcdKey::Config config = AcdKey::LAT, 
		   int gemDeltaEventTimeCut = 0,
		   bool requireBothPmtsInTiles = false,
		   bool requireSinglesInTiles = false);
  
  virtual ~AcdCalibLoop_OVL();  

protected:
  
  /// setup input data
  Bool_t attachChains();

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId, Double_t& timeStamp);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);

private:

  /// pointer to a EventOverlay
  EventOverlay* m_eventOverlay;
  
  /// Histograms for data in PHA
  AcdHistCalibMap* m_phaHists;

  /// Histograms for data in MeV
  AcdHistCalibMap* m_MeVHists;

  /// GemDeltaEventTime cut
  int m_deltaEventTimeCut;

  /// Require signals from both PMTs in tiles
  bool m_requireBothPmtsInTiles;

  /// Take only single PMT hits in tiles
  bool m_requireSinglesInTiles;

};

#endif
