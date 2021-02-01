#ifndef AcdNoiseLoop_h
#define AcdNoiseLoop_h 

// base classs
#include "AcdCalibBase.h"

// local includes
#include "./AcdGarcGafeHits.h"

// stl includes
#include <iostream>

class TFile;
class TTree;
class TBranch;

/** 
 * @class AcdNoiseLoop
 *
 * @brief AcdCalibration class that loop on Svac files
 *
 * This is used for the following calibrations:
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdNoiseLoop.h,v 1.1 2008/09/05 22:57:34 echarles Exp $
 */

class AcdNoiseLoop : public AcdCalibBase {

public :

  /// Standard ctor, where user provides the input data
  AcdNoiseLoop(AcdCalibData::CALTYPE t, 
	       TChain* svacChain, 
	       AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdNoiseLoop();  
 
  TFile* makeOutput(const char* fileName);  

protected:

  /// setup input data
  Bool_t attachChains();

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId, Double_t& timeStamp);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);

  /// Get the new branchs when switching trees in a chain
  Bool_t getBranches();

private:

  /// GEM primatives
  Int_t m_GemConditionsWord;
  /// Branch for FswDGNState
  TBranch* b_GemConditionsWord;

  /// Ticks since last read event
  UInt_t m_GemDeltaEventTime;
  /// Branch for GemDeltaEventTime
  TBranch* b_GemDeltaEventTime;

  /// Ticks since last window open
  UInt_t m_GemDeltaWindowOpenTime;
  /// Branch for GemDeltaWindowOpenTime
  TBranch* b_GemDeltaWindowOpenTime;

  /// Number of tracks, sed to select only 1-TRK events
  Int_t m_TkrNumTracks;
  /// Branch for TkrNumTracks
  TBranch* b_TkrNumTracks;

  /// Energy in CAL, used to flag events that missed the CAL
  Float_t m_CalEnergyRaw;
  /// Branch for CalEnergyRaw
  TBranch* b_CalEnergyRaw;
  
  /// Calibrated MIP equivalent signal seem by ID,PMT
  Float_t m_AcdMips[604][2];
  /// Branch for AcdMips
  TBranch* b_AcdMips;

  /// ACD HitMap bits equivalent signal seem by ID,PMT
  Int_t m_AcdHitMap[604][2];
  /// Branch for AcdMips
  TBranch* b_AcdHitMap;

  /// Time stamp for event
  Double_t m_timeStamp;
  /// Branch for timeStamp
  TBranch* b_timeStamp;

  /// Number of ACD Tiles with either PMT above 0p1 mips
  Int_t m_nAcdHit_0p1;

  /// Number of ACD Tiles with either PMT above 0p25 mips
  Int_t m_nAcdHit_0p25;

  /// Number of ACD Tiles with either hitmap bit set
  Int_t m_nAcdVeto;

  /// Output File
  TFile* m_fout;

  /// Output Tree
  TTree* m_tout;  

  /// Event index in current tree
  Long64_t m_idx;

};

#endif
