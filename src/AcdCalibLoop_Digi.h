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
 *   - Pedestals (low and high range)
 *   - Veto thresholds
 *   - CNO thresholds
 *   - Range crossover
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibLoop_Digi.h,v 1.7 2008/08/01 00:08:25 echarles Exp $
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

  /// Histograms for pedestal calibrations
  AcdHistCalibMap* m_rawHists;
  /// Histograms for veto and cno set point calibrations
  AcdHistCalibMap* m_threshHists;
  /// Histograms for range crossover calibrations
  AcdHistCalibMap* m_rangeHists;
  
  /// Input pedestal calibration
  AcdCalibMap* m_peds;
  /// Input range calibration
  AcdCalibMap* m_ranges;

  /// Used to work in GARC:GAFE space instead of TILE:PMT space, Needed for CNO
  AcdGarcGafeHits m_garcGafeHits;  
    
};

#endif
