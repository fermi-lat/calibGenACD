#ifndef AcdCalibLoop_Svac_h
#define AcdCalibLoop_Svac_h 

// base classs
#include "AcdCalibBase.h"

// stl includes
#include <iostream>


class AcdCalibLoop_Svac : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  // and optionally the name of the output ROOT histogram file
  AcdCalibLoop_Svac(TChain* svacChain, Bool_t correctPathLength = kTRUE, AcdMap::Config config = AcdMap::LAT);
  
  virtual ~AcdCalibLoop_Svac();  
  
protected:

  Bool_t attachChains();

  // get reconstruction direction 
  void getFitDir();
    
  // 
  void fillGainHistCorrect(unsigned id, float pathLength);

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);


private:

  // Local options
  Bool_t      m_correctPathLength;

  //  Variables that we need
  Int_t  m_AcdPha[604][2];
  Int_t  m_AcdRange[604][2];

  Int_t m_AcdNumTkrIntSec;
  Int_t m_AcdTkrIntSecTileId[20];
  Int_t m_AcdTkrIntSecTkrIndex[20];
  Float_t m_AcdTkrIntSecPathLengthInTile[20];    

  // 
  AcdHistCalibMap* m_gainHists;
        
};

#endif
