#ifndef AcdMuonTkrCalib_h
#define AcdMuonTkrCalib_h 

#include "AcdCalibBase.h"

#include "./AcdGainFit.h"
#include "./AcdPedestalFit.h"

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
  AcdMuonTkrCalib(TChain* digiChain, TChain *reconChain);
  
  virtual ~AcdMuonTkrCalib();  

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
  void fillGainHistCorrect(const AcdTkrIntersection& inter, const AcdDigi& digi);

  // return the total number of events in the chains
  virtual int getTotalEvents() const { 
    if ( m_digiChain != 0 ) { return (int)(m_digiChain->GetEntries()); }
    if ( m_reconChain != 0 ) { return (int)(m_reconChain->GetEntries()); }
    return 0;
  } 

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);


private:

  /// TChain input
  TChain      *m_digiChain;
  TChain      *m_reconChain;

  /// pointer to a ReconEvent
  DigiEvent* m_digiEvent;

  /// pointer to a ReconEvent
  ReconEvent* m_reconEvent;

  // 
  AcdHistCalibMap* m_gainHists;
    
  AcdGainFitMap* m_gains;
  AcdPedestalFitMap* m_peds;

  ClassDef(AcdMuonTkrCalib,0) ;
    
};

#endif
