#ifndef AcdCalibLoop_Recon_h
#define AcdCalibLoop_Recon_h 

// base class
#include "AcdCalibBase.h"

// stl includes
#include <iostream>

// forward declares
class AcdTkrIntersection;
class AcdDigi;
class DigiEvent;
class ReconEvent;


class AcdCalibLoop_Recon : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  // and optionally the name of the output ROOT histogram file
  AcdCalibLoop_Recon(TChain* digiChain, TChain *reconChain, Bool_t correctPathLength = kTRUE, AcdMap::Config = AcdMap::LAT);
  
  virtual ~AcdCalibLoop_Recon();  

protected:

  Bool_t attachChains();

  // get reconstruction direction 
  void getFitDir();
    
  // 
  void fillGainHistCorrect(const AcdTkrIntersection& inter, const AcdDigi& digi);

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);


private:

  // Local options
  Bool_t      m_correctPathLength;

  /// pointer to a ReconEvent
  DigiEvent* m_digiEvent;

  /// pointer to a ReconEvent
  ReconEvent* m_reconEvent;

  // 
  AcdHistCalibMap* m_gainHists;
};

#endif
