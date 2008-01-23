#ifndef AcdCalibLoop_Recon_h
#define AcdCalibLoop_Recon_h 

// base class
#include "AcdCalibBase.h"

// stl includes
#include <iostream>

// forward declares
class AcdTkrIntersection;
class AcdDigi;
class DigiEvent;
class ReconEvent;

/** 
 * @class AcdCalibLoop_Recon
 *
 * @brief AcdCalibration class that loop on Recon and Digi
 *
 * This is used for the following calibrations:
 *   - Gains with pathlength correction from recon objects
 *
 * @author Eric Charles
 * $Header$
 */

class AcdCalibLoop_Recon : public AcdCalibBase {

public :

  /// Standard ctor, where user provides the input data
  AcdCalibLoop_Recon(TChain* digiChain, TChain *reconChain, Bool_t correctPathLength = kTRUE, AcdMap::Config = AcdMap::LAT);
  
  virtual ~AcdCalibLoop_Recon();  

protected:

  /// setup input data
  Bool_t attachChains();

  /// get reconstructed track direction 
  void getFitDir();
    
  /// fill a histogram, possibly using the pathlength correction
  void fillGainHistCorrect(const AcdTkrIntersection& inter, const AcdDigi& digi);

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);


private:

  /// flag for using the pathlength correction
  Bool_t      m_correctPathLength;

  /// pointer to a ReconEvent
  DigiEvent* m_digiEvent;

  /// pointer to a ReconEvent
  ReconEvent* m_reconEvent;
  
  // Various histograms to fill
  AcdHistCalibMap* m_gainHists;
};

#endif
