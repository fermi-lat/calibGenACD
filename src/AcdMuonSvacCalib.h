#ifndef AcdMuonSvacCalib_h
#define AcdMuonSvacCalib_h 

#include "AcdCalibBase.h"

#include "./AcdGainFit.h"
#include "./AcdPedestalFit.h"

#include "TTree.h"
#include "TH1.h"
#include "TChain.h"
#include <iostream>


class AcdMuonSvacCalib : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  // and optionally the name of the output ROOT histogram file
  AcdMuonSvacCalib(TChain* svacChain, Bool_t correctPathLength = kTRUE);
  
  virtual ~AcdMuonSvacCalib();  

  // this just call down to the fitAll() routine in the fitter
  AcdGainFitMap* fitGains(AcdGainFit& fitter);
  
  Bool_t readPedestals(const char* fileName);

  /// for writing output files
  virtual void writeXmlSources(ostream& os) const;
  virtual void writeTxtSources(ostream& os) const;

protected:

  Bool_t attachChains();

  // get reconstruction direction 
  void getFitDir();
    
  // 
  void fillGainHistCorrect(unsigned id, float pathLength);

  // return the total number of events in the chains
  virtual int getTotalEvents() const { 
    if ( m_svacChain != 0 ) { return (int)(m_svacChain->GetEntries()); }
    return 0;
  } 

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);


private:

  // Local options
  Bool_t      m_correctPathLength;

  /// TChain input
  TChain      *m_svacChain;

  //  Variables that we need
  Int_t  m_AcdPha[604][2];
  Int_t  m_AcdRange[604][2];

  Int_t m_AcdNumTkrIntSec;
  Int_t m_AcdTkrIntSecTileId[20];
  Int_t m_AcdTkrIntSecTkrIndex[20];
  Float_t m_AcdTkrIntSecPathLengthInTile[20];    

  // 
  AcdHistCalibMap* m_gainHists;
    
  AcdGainFitMap* m_gains;
  AcdPedestalFitMap* m_peds;

  ClassDef(AcdMuonSvacCalib,0) ;
    
};

#endif
