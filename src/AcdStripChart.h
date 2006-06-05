#ifndef AcdStripChart_h
#define AcdStripChart_h 

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


class AcdStripChart : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  // and optionally the name of the output ROOT histogram file
  AcdStripChart(TChain* digiChain, UInt_t nBins);
  
  virtual ~AcdStripChart();  

  Bool_t readPedestals(const char* fileName);

  /// for writing output files
  virtual void writeXmlSources(ostream& os) const;
  virtual void writeTxtSources(ostream& os) const;

protected:

  Bool_t attachChains();

  // 
  void accumulate(int ievent, const AcdDigi& digi);

  // return the total number of events in the chains
  virtual int getTotalEvents() const { 
    if ( m_digiChain != 0 ) { return (int)(m_digiChain->GetEntries()); }
    return 0;
  } 

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);


private:

  // Local stash
  UInt_t m_nBins;
  UInt_t m_nEvtPerBin;
  mutable UInt_t m_currentCount;
  mutable UInt_t m_currentBin;

  /// TChain input
  TChain      *m_digiChain;

  /// pointer to a ReconEvent
  DigiEvent* m_digiEvent;

  // 
  AcdHistCalibMap* m_phaStrip;

  AcdPedestalFitMap* m_peds;
  
  mutable std::map<UInt_t,Double_t> m_run_N;
  mutable std::map<UInt_t,Double_t> m_run_X;
  mutable std::map<UInt_t,Double_t> m_run_X2;

  ClassDef(AcdStripChart,0) ;
    
};

#endif
