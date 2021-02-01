#ifndef AcdCalibLoop_Linked_h
#define AcdCalibLoop_Linked_h 

// base class
#include "AcdCalibBase.h"

// local includes
#include "./AcdGarcGafeHits.h"

// stl includes
#include <iostream>

// forward declares
class TH2;
class TChain;
class DigiEvent;
class ReconEvent;

/** 
 * @class AcdCalibLoop_Linked
 *
 * @brief AcdCalibration class that loop on Digis, Recon and Merit
 *
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibLoop_Linked.h,v 1.1 2014/02/14 03:29:00 echarles Exp $
 */

class AcdCalibLoop_Linked : public AcdCalibBase {

public :
  
  /// Standard ctor, where user provides the input data
  AcdCalibLoop_Linked(AcdCalibData::CALTYPE t, 
		      TChain *digiChain, 
		      TChain *reconChain, 
		      TChain *meritChain,
		      Bool_t requirePeriodic = kFALSE, 
		      AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdCalibLoop_Linked();  

protected:
  
  /// setup input data
  Bool_t attachChains();

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId, Double_t& timeStamp);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);

  /// get the MIP value for one pmt
  Bool_t getMipValue(UInt_t key, UInt_t range, UInt_t pha, Float_t& mips);

  /// Fill variables from digi information
  void fillDigiInfo(Bool_t& used);

  /// Fill gain variables from reco information
  void fillRecoGainInfo(Bool_t& used);


private:

  /// pointer to a DigiEvent
  DigiEvent* m_digiEvent;

  /// pointer to a ReconEvent
  ReconEvent* m_reconEvent;
    
  /// Take only periodic events
  Bool_t m_requirePeriodic;

  
  /// Histograms for pedestals
  AcdHistCalibMap* m_pedHists;
  /// Histograms for MIP peak, carbon peaks, ribbon attenuation and gain checking)
  AcdHistCalibMap* m_peakHists;
  /// Histograms for range crossover calibration checking
  AcdHistCalibMap* m_rangeHists;
  /// Histograms for veto set point calibration checking
  AcdHistCalibMap* m_threshHists;

  
    
};

#endif
