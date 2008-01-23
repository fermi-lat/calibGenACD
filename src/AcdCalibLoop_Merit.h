#ifndef AcdCalibLoop_Merit_h
#define AcdCalibLoop_Merit_h 


// base class
#include "AcdCalibBase.h"

// stl includes
#include <iostream>

// forward declares
class AcdTkrIntersection;
class AcdDigi;
class DigiEvent;
class ReconEvent;
class TTree;

/** 
 * @class AcdCalibLoop_Merit
 *
 * @brief Deprecated!!
 *
 * @author Eric Charles
 * $Header$
 */

class AcdCalibLoop_Merit : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  AcdCalibLoop_Merit(TChain& digiChain, TChain& reconChain, TChain& meritChain);
  
  virtual ~AcdCalibLoop_Merit();  

  inline TTree* outputTree() { return m_outputTree; }

protected:

  Bool_t attachChains();

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);

  Float_t toMip(UInt_t channel, Int_t pha) const;

private:

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
       
};

#endif
