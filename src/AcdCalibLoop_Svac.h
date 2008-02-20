#ifndef AcdCalibLoop_Svac_h
#define AcdCalibLoop_Svac_h 

// base classs
#include "AcdCalibBase.h"

// stl includes
#include <iostream>

/** 
 * @class AcdCalibLoop_Svac
 *
 * @brief AcdCalibration class that loop on Digis
 *
 * This is used for the following calibrations:
 *   - Gains with pathlength correction
 *
 * @author Eric Charles
 * $Header$
 */

class AcdCalibLoop_Svac : public AcdCalibBase {

public :

  /// Standard ctor, where user provides the input data
  AcdCalibLoop_Svac(AcdCalibData::CALTYPE t, TChain* svacChain, Bool_t correctPathLength = kTRUE, AcdMap::Config config = AcdMap::LAT);
  
  virtual ~AcdCalibLoop_Svac();  
  
protected:

  /// setup input data
  Bool_t attachChains();

  /// get reconstructed track direction 
  void getFitDir();
    
  /// fill a histogram, possibly using the pathlength correction
  void fillGainHistCorrect(unsigned id, float pathLength, unsigned iISect);

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);


private:

  /// flag for using the pathlength correction
  Bool_t      m_correctPathLength;

  //  Variables that we need from svac tuple
  Int_t  m_AcdPha[604][2];
  Int_t  m_AcdRange[604][2];
  Int_t  m_AcdRibbonCount;

  Int_t m_AcdNumTkrIntSec;
  Int_t m_AcdTkrIntSecTileId[20];
  Int_t m_AcdTkrIntSecTkrIndex[20];
  Float_t m_AcdTkrIntSecPathLengthInTile[20];    

  Float_t m_AcdTkrIntSecGlobalX[20];    
  Float_t m_AcdTkrIntSecGlobalY[20];    
  Float_t m_AcdTkrIntSecGlobalZ[20];    

  // 
  AcdHistCalibMap* m_gainHists;
        
};

#endif
