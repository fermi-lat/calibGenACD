#ifndef AcdCalibLoop_Digi_h
#define AcdCalibLoop_Digi_h 

// base class
#include "AcdCalibBase.h"

// local includes
#include "./AcdCalibEnum.h"
#include "./AcdGarcGafeHits.h"

// stl includes
#include <iostream>

// forward declares
class TH2;
class TChain;
class DigiEvent;

class AcdCalibLoop_Digi : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the input data
  AcdCalibLoop_Digi(AcdCalib::CALTYPE t, TChain *digiChain, Bool_t requirePeriodic = kFALSE, AcdMap::Config config = AcdMap::LAT);
  
  virtual ~AcdCalibLoop_Digi();  

  TH2* getHitMapHist() {
    return m_hitMapHist;
  }
  TH2* getCondArrHist() {
    return m_condArrHist;
  }

  AcdHistCalibMap* makeRatioPlots();

protected:

  Bool_t attachChains();

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);

  void compareDigiToGem();

  void fillCnoData();

private:

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
  
  AcdCalibMap* m_peds;
  AcdCalibMap* m_ranges;

  AcdGarcGafeHits m_garcGafeHits;  

  ClassDef(AcdCalibLoop_Digi,0) ;
    
};

#endif
