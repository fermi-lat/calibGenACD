#ifndef AcdMeritCalib_h
#define AcdMeritCalib_h 

#include "AcdCalibBase.h"

#include "TTree.h"
#include "TH1.h"
#include "TChain.h"
#include <iostream>

#include "./AcdGainFit.h"
#include "./AcdPedestalFit.h"

class AcdTkrIntersection;
class AcdDigi;
class DigiEvent;
class ReconEvent;


class AcdMeritCalib : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  AcdMeritCalib(TChain& digiChain, TChain& reconChain, TChain& meritChain);
  
  virtual ~AcdMeritCalib();  

  /// for writing output files
  virtual void writeXmlHeader(ostream& os) const;
  virtual void writeTxtHeader(ostream& os) const;
  
  Bool_t readPedestals(const char* fileName);
  Bool_t readGains(const char* fileName);    

  inline TTree* outputTree() { return m_outputTree; }

protected:

  Bool_t attachChains();

  // return the total number of events in the chains
  virtual int getTotalEvents() const { 
    if ( m_digiChain != 0 ) { return (int)(m_digiChain->GetEntries()); }
    if ( m_reconChain != 0 ) { return (int)(m_reconChain->GetEntries()); }
    if ( m_meritChain != 0 ) { return (int)(m_meritChain->GetEntries()); }
    return 0;
  } 

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);

  Float_t toMip(UInt_t channel, Int_t pha) const;

private:

  /// TChain input
  TChain      *m_digiChain;
  TChain      *m_reconChain;
  TChain      *m_meritChain;

  /// pointer to a ReconEvent
  DigiEvent* m_digiEvent;

  /// pointer to a ReconEvent
  ReconEvent* m_reconEvent;

  /// Some stuff from merit
  Float_t m_TkrNumTracks;    // why is this a float?
  Float_t m_Tkr1SSDVeto;     // why is this a float?
  Float_t m_Tkr1Hits;        // why is this a float?
  Float_t m_Tkr1Chisq;
  Float_t m_CalMIPRatio;     

  // stuff for output  
  Int_t m_runIdOut;
  Int_t m_evtIdOut;
  Int_t m_acdIdOut;
  Float_t m_weight;
  Float_t m_sToX;
  Float_t m_doca;
  Float_t m_gX;
  Float_t m_gY;
  Float_t m_gZ;
  Float_t m_mipA;
  Float_t m_mipB;
  Int_t m_hit;

  TTree* m_outputTree;
 
  AcdGainFitMap* m_gains;
  AcdPedestalFitMap* m_peds;
  
  ClassDef(AcdMeritCalib,0) ;
    
};

#endif
