#ifndef AcdMuonRoiCalib_h
#define AcdMuonRoiCalib_h 

#include "AcdCalibBase.h"

#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TChain.h"
#include <iostream>
#include "./AcdGainFit.h"
#include "./AcdPedestalFit.h"
#include "./AcdVetoFit.h"

class DigiEvent;

class AcdMuonRoiCalib : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the input data
  AcdMuonRoiCalib(TChain *digiChain, Bool_t requirePeriodic = kFALSE);
  
  virtual ~AcdMuonRoiCalib();  

  // these two just call down to the fitAll() routines in the fitters
  AcdPedestalFitMap* fitPedestals(AcdPedestalFit& fitter);
  AcdGainFitMap* fitGains(AcdGainFit& fitter);
  AcdVetoFitMap* fitVetos(AcdVetoFit& fitter);

  TH2* getHitMapHist() {
    return m_hitMapHist;
  }
  TH2* getCondArrHist() {
    return m_condArrHist;
  }

  AcdHistCalibMap* makeVetoRatio();

  /// for writing output files
  virtual void writeXmlSources(ostream& os) const;
  virtual void writeTxtSources(ostream& os) const;

protected:

  Bool_t attachChains();

  // return the total number of events in the chains
  virtual int getTotalEvents() const { 
    if ( m_digiChain != 0 ) { return (int)(m_digiChain->GetEntries()); }    
    return 0;
  } 

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);

  void compareDigiToGem();

private:

  /// Optional TChain input
  TChain     *m_digiChain;
  
  /// pointer to a DigiEvent
  DigiEvent* m_digiEvent;
  
  // 
  Bool_t m_requirePeriodic;

  // 
  AcdHistCalibMap* m_pedHists;
  AcdHistCalibMap* m_gainHists;
  AcdHistCalibMap* m_unPairHists;
  AcdHistCalibMap* m_rawHists;
  AcdHistCalibMap* m_vetoHists;
  AcdHistCalibMap* m_vfHists;

  TH2* m_hitMapHist;
  TH2* m_condArrHist;
  
  AcdGainFitMap* m_gains;
  AcdPedestalFitMap* m_peds;
  AcdVetoFitMap* m_vetos;

  ClassDef(AcdMuonRoiCalib,0) ;
    
};

#endif
