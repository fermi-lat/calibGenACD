#ifndef muonCalib_h
#define muonCalib_h 

#include "AcdCalibBase.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TChain.h"
#include <iostream>

class DigiEvent;

class muonCalib : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  // and optionally the name of the output ROOT histogram file
  muonCalib(TChain *digiChain, TChain *meritChain = 0, 
	    const char *histFileName="Histograms.root");
  
  virtual ~muonCalib();  
  
  // reset for next Go to start at beginning of file 
  void rewind() { m_startEvent = 0; }; 

  /// process events
  void go(int numEvents=100000); 

  // 
  virtual Float_t reconCorrection(UInt_t face  ) { 
    if ( face == 0 ) return (-1*m_reconDirZ);
    return sqrt( 1. - m_reconDirZ*m_reconDirZ );
  }
  
protected:

  Bool_t attachChains();

  // get reconstruction direction 
  void getFitDir();
  
  // select events used to calibrate gain
  virtual Bool_t failCuts();
  
  // process digi data in ACD
  void digiAcd();

private:

  /// Optional TChain input
  TChain      *m_digiChain, *m_meritChain;
  
  /// pointer to a DigiEvent
  DigiEvent* m_digiEvent;
    
  /// starting event number
  Int_t m_startEvent;
  
  // reconstructed event direction along the z axis, defaulted as -9999
  Double_t m_reconDirZ;
  
  UInt_t m_nVals;

  ClassDef(muonCalib,0) ;
    
};

#endif
