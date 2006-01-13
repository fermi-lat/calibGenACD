#ifndef AcdMuonTkrCalib_h
#define AcdMuonTkrCalib_h 

#include "AcdCalibBase.h"

#include "TTree.h"
#include "TH1.h"
#include "TChain.h"
#include <iostream>

class AcdTkrIntersection;
class AcdDigi;
class DigiEvent;
class ReconEvent;


class AcdMuonTkrCalib : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  // and optionally the name of the output ROOT histogram file
  AcdMuonTkrCalib(TChain* digiChain, TChain *reconChain, const char *histFileName="Histograms.root");
  
  virtual ~AcdMuonTkrCalib();  
  
  // reset for next Go to start at beginning of file 
  void rewind() { m_startEvent = 0; }; 

  /// process events
  void go(int numEvents=100000); 

  // 
  
protected:

  Bool_t attachChains();

  // get reconstruction direction 
  void getFitDir();
  
  
  // process recon data in ACD
  void doAcd();

  // 
  void fillGainHistCorrect(const AcdTkrIntersection& inter, const AcdDigi& digi);

private:

  /// TChain input
  TChain      *m_digiChain;
  TChain      *m_reconChain;
  
  /// pointer to a ReconEvent
  DigiEvent* m_digiEvent;

  /// pointer to a ReconEvent
  ReconEvent* m_reconEvent;
    
  /// starting event number
  Int_t m_startEvent;
  
  UInt_t m_nVals;

  ClassDef(AcdMuonTkrCalib,0) ;
    
};

#endif
