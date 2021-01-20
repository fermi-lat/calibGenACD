#ifndef AcdCalibLoop_Recon_h
#define AcdCalibLoop_Recon_h 

// base class
#include "AcdCalibBase.h"

//#include "reconRootData/AcdTkrHitPocaV2.h"
#include "AcdUtil/AcdTileFuncs.h"
#include "AcdUtil/AcdUtilFuncs.h"

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
 * @class AcdCalibLoop_Recon
 *
 * @brief AcdCalibration class that loop on Digis, Recon and Merit
 *
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibLoop_Recon.h,v 1.1 2014/02/14 03:29:00 echarles Exp $
 */

class AcdCalibLoop_Recon : public AcdCalibBase {

public :
  
  /// Standard ctor, where user provides the input data
  AcdCalibLoop_Recon(AcdCalibData::CALTYPE t, 
		      TChain *digiChain, 
		      TChain *reconChain, 
//		      TChain *meritChain,
		      Bool_t requirePeriodic = kFALSE, 
		      AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdCalibLoop_Recon();  

protected:
  
  /// setup input data
  Bool_t attachChains();

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId, Double_t& timeStamp);    

  /// Try to use an event for calibration
//  virtual void useEvent(Bool_t& used, std::ofstream& outfile, int ievent);
  virtual void useEvent(Bool_t& used);

  /// get the MIP value for one pmt
  Bool_t getMipValue(UInt_t key, UInt_t range, UInt_t pha, Float_t& mips);

  /// Fill variables from digi information
//  void fillDigiInfo(Bool_t& used);

//  void reconId();

//  void setId(const std::vector<AcdTkrHitPocaV2*> bestTrackUp, unsigned int &retId, bool findRibbon);

//  void findId(const std::vector<AcdTkrHitPocaV2*>& vec, const AcdId &retId, bool findRibbon);

  /// Fill gain variables from reco information
//  void fillRecoGainInfo(Bool_t& used, std::ofstream& outfile, int ievent);
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

//  unsigned int ACD_TileIdRecon;
//  unsigned int ACD_RibbonIdRecon;
    
};

#endif
