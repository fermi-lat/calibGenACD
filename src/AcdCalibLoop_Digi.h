#ifndef AcdCalibLoop_Digi_h
#define AcdCalibLoop_Digi_h 

#include "AcdCalibBase.h"

#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TChain.h"
#include <iostream>
#include "./AcdGainFit.h"
#include "./AcdPedestalFit.h"
#include "./AcdVetoFit.h"
#include "./AcdCnoFit.h"
#include "./AcdRangeFit.h"
#include "./AcdGarcGafeHits.h"

class DigiEvent;

class AcdCalibLoop_Digi : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the input data
  AcdCalibLoop_Digi(CALTYPE t, TChain *digiChain, Bool_t requirePeriodic = kFALSE, AcdMap::Config config = AcdMap::LAT);
  
  virtual ~AcdCalibLoop_Digi();  

  // these just call down to the fitAll() routines in the fitters
  AcdPedestalFitMap* fitPedestals(AcdPedestalFit& fitter);
  AcdGainFitMap* fitGains(AcdGainFit& fitter);
  AcdVetoFitMap* fitVetos(AcdVetoFit& fitter);
  AcdCnoFitMap* fitCnos(AcdCnoFit& fitter);
  AcdRangeFitMap* fitRanges(AcdRangeFit& fitter);

  TH2* getHitMapHist() {
    return m_hitMapHist;
  }
  TH2* getCondArrHist() {
    return m_condArrHist;
  }

  AcdHistCalibMap* makeRatioPlots();

  /// for writing output files
  virtual void writeXmlSources(DomElement& node) const;
  virtual void writeTxtSources(ostream& os) const;

  Bool_t readPedestals(const char* fileName);

  Bool_t readRanges(const char* fileName);

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

  void fillCnoData();

private:

  /// Optional TChain input
  TChain     *m_digiChain;
  
  /// pointer to a DigiEvent
  DigiEvent* m_digiEvent;
  
  // 
  Bool_t m_requirePeriodic;

  // 
  AcdHistCalibMap* m_rawHists;
  AcdHistCalibMap* m_gainHists;
  AcdHistCalibMap* m_vetoHists;
  AcdHistCalibMap* m_fracHists;
  AcdHistCalibMap* m_rangeHists;
  AcdHistCalibMap* m_unPairHists;

  TH2* m_hitMapHist;
  TH2* m_condArrHist;
  
  AcdPedestalFitMap* m_peds;
  AcdGainFitMap* m_gains;
  AcdVetoFitMap* m_vetos;
  AcdCnoFitMap* m_cnos;
  AcdRangeFitMap* m_ranges;

  AcdGarcGafeHits m_garcGafeHits;  

  ClassDef(AcdCalibLoop_Digi,0) ;
    
};

#endif
