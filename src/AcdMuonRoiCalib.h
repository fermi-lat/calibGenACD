#ifndef AcdMuonRoiCalib_h
#define AcdMuonRoiCalib_h 

#include "AcdCalibBase.h"

#include "TTree.h"
#include "TH1.h"
#include "TChain.h"
#include <iostream>

class DigiEvent;

class AcdMuonRoiCalib : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  // and optionally the name of the output ROOT histogram file
  AcdMuonRoiCalib(TChain *digiChain, TChain *meritChain = 0, 
	    const char *histFileName="Histograms.root");
  
  virtual ~AcdMuonRoiCalib();  
  
protected:

  Bool_t attachChains();

  // get reconstruction direction 
  void getFitDir();
  
  // correct for pedestal, direction
  void fillGainHistCorrect(Int_t id, Int_t pmt, Int_t range, Int_t pha);

  // return the total number of events in the chains
  virtual int getTotalEvents() const { 
    if ( m_digiChain != 0 ) { return (int)(m_digiChain->GetEntries()); }    
    if ( m_meritChain!= 0 ) { return (int)(m_meritChain->GetEntries()); }
    return 0;
  } 

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);


private:

  /// Optional TChain input
  TChain     *m_digiChain, *m_meritChain;
  
  /// pointer to a DigiEvent
  DigiEvent* m_digiEvent;
    
  // reconstructed event direction along the x,y,z axis, defaulted as -9999
  Float_t m_reconDirX;
  Float_t m_reconDirY;
  Float_t m_reconDirZ;
 
  ClassDef(AcdMuonRoiCalib,0) ;
    
};

#endif
