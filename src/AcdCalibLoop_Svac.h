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
  AcdCalibLoop_Svac(AcdCalibData::CALTYPE t, TChain* svacChain, 
		    Bool_t correctPathLength = kTRUE, 
		    Bool_t calGCRSelect = kFALSE,
		    AcdMap::Config config = AcdMap::LAT);
  
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
			   int& runId, int& evtId, Double_t& timeStamp);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);

  /// Run the GCR selection
  Bool_t runGCRSelect();


  /// Get the range cross-over data
  Bool_t getRangeData(UInt_t key, Float_t& low_max, Float_t& high_min) const;

  /// get the high range pedestal for a channel
  float getPedsHigh(UInt_t key) const;

  /// Transform the data into a single range
  Bool_t singleRange(UInt_t key, Int_t pha, Int_t range, Float_t& val) const;


private:

  /// flag for using the pathlength correction
  Bool_t      m_correctPathLength;

  /// flag for using the CAL to select CarbonIona
  Bool_t      m_calGCRSelect;

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
  

  Int_t m_TrgEngineGem;
  Float_t m_Tkr1ZDir;
  Float_t m_CalXtalEne[16][8][12];

  Double_t m_timeStamp;

  // 
  AcdHistCalibMap* m_gainHists;
        
};

#endif
