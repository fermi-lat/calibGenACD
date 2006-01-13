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
  
  // reset for next Go to start at beginning of file 
  void rewind() { m_startEvent = 0; }; 

  /// process events
  void go(int numEvents=100000); 

protected:

  Bool_t attachChains();

  // get reconstruction direction 
  void getFitDir();
  
  // select events used to calibrate gain
  virtual Bool_t failCuts();
  
  // process digi data in ACD
  void digiAcd();

  // correct for pedestal, direction
  void fillGainHistCorrect(Int_t id, Int_t pmt, Int_t range, Int_t pha);

private:

  /// Optional TChain input
  TChain      *m_digiChain, *m_meritChain;
  
  /// pointer to a DigiEvent
  DigiEvent* m_digiEvent;
    
  /// starting event number
  Int_t m_startEvent;
  
  // reconstructed event direction along the x,y,z axis, defaulted as -9999
  Float_t m_reconDirX;
  Float_t m_reconDirY;
  Float_t m_reconDirZ;
  
  UInt_t m_nVals;

  ClassDef(AcdMuonRoiCalib,0) ;
    
};

#endif
