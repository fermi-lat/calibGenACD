#ifndef AcdEfficLoop_h
#define AcdEfficLoop_h 

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
 * @class AcdEfficLoop
 *
 * @brief AcdCalibration class that loop on Svac files
 *
 * This is used for the following calibrations:
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdEfficLoop.h,v 1.2 2010/06/24 23:52:42 echarles Exp $
 */

class AcdEfficLoop : public AcdCalibBase {

public :

  /// Standard ctor, where user provides the input data
  AcdEfficLoop(AcdCalibData::CALTYPE t, 
	       TChain* svacChain, TChain* meritChain,
	       AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdEfficLoop();  
  
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

  /// Diagnostic filter state, used to select only DGN events
  Int_t m_FswDGNState;
  /// Branch for FswDGNState
  TBranch* b_FswDGNState;

  /// Number of tracks, sed to select only 1-TRK events
  Int_t m_TkrNumTracks;
  /// Branch for TkrNumTracks
  TBranch* b_TkrNumTracks;

  /// Gamma prob, assigned by TKR
  Float_t m_CTBTKRGamProb_in;
  Double_t m_CTBTKRGamProb;
  /// Branch for CTBTKRGamProb
  TBranch* b_CTBTKRGamProb;

  /// Gamma prob, assigned by CAL
  Float_t m_CTBCALGamProb_in;
  Double_t m_CTBCALGamProb;
  /// Branch for CTBTKRGamProb
  TBranch* b_CTBCALGamProb;

  /// Gamma prob, assigned by ACD/CPF
  Float_t m_CTBCPFGamProb_in;
  Double_t m_CTBCPFGamProb;
  /// Branch for CTBTKRGamProb
  TBranch* b_CTBCPFGamProb;

  /// CTBClassLevel
  Float_t m_CTBClassLevel_in;
  Int_t m_CTBClassLevel;
  /// Branch for CTBTKRGamProb
  TBranch* b_CTBClassLevel;

  /// CTBBestEnergyProb
  Float_t m_CTBBestEnergyProb_in;
  Double_t m_CTBBestEnergyProb;
  /// branch for CTBBestEnergyProb
  TBranch* b_CTBBestEnergyProb;

  /// CTBBestEnergy
  Float_t m_CTBBestLogEnergy_in;
  Double_t m_CTBBestLogEnergy;
  /// branch for CTBBestEnergy
  TBranch* b_CTBBestLogEnergy;

  /// CTBCORE
  Float_t m_CTBCORE_in;
  Double_t m_CTBCORE;
  /// branch for CTBCORE
  TBranch* b_CTBCORE;

  /// Ratio to MIP energy
  Float_t m_CalMIPRatio_in;
  Double_t m_CalMIPRatio;
  /// Branch for CalEnergyRaw
  TBranch* b_CalMIPRatio;

  /// Energy in CAL, used to flag events that missed the CAL
  Float_t m_CalEnergyRaw_in;
  Double_t m_CalEnergyRaw;
  /// Branch for CalEnergyRaw
  TBranch* b_CalEnergyRaw;

  /// Number of rad lengths in CAL, used to flag events that clipped the CAL
  Float_t m_CalCsIRLn_in;
  Double_t m_CalCsIRLn;
  /// Branch for CalCsIRLn
  TBranch* b_CalCsIRLn;

  /// Number of tracker layers with hits above start of track
  Float_t m_Tkr1SSDVeto_in;
  Double_t m_Tkr1SSDVeto;
  /// Branch for Tkr1SSDVeto
  TBranch* b_Tkr1SSDVeto;

  /// Kalman filter energy for best track
  Float_t m_Tkr1KalEne_in;  
  Double_t m_Tkr1KalEne;
  /// Branch for Tkr1KalEne
  TBranch* b_Tkr1KalEne;
  
  /// Number of fthis on the best track
  Int_t m_Tkr1NumHits_in;
  Double_t m_Tkr1NumHits;
  /// Branch for Tkr1NumHits
  TBranch* b_Tkr1NumHits;
  
  /// Calibrated MIP equivalent signal seem by ID,PMT
  Float_t m_AcdMips[604][2];
  /// Branch for AcdMips
  TBranch* b_AcdMips;

  /// X of point where track crosses ACD Cube
  Float_t m_AcdTkrPointX[2];
  /// Branch for AcdTkrPointX
  TBranch* b_AcdTkrPointX;

  /// Y of point where track crosses ACD Cube
  Float_t m_AcdTkrPointY[2];
  /// Branch for AcdTkrPointY
  TBranch* b_AcdTkrPointY;

  /// Z of point where track crosses ACD Cube
  Float_t m_AcdTkrPointZ[2];
  /// Branch for AcdTkrPointZ
  TBranch* b_AcdTkrPointZ;

  /// Which face of ACD Cube track crossed
  Int_t m_AcdTkrPointFace[2];
    /// Branch for AcdTkrPointFace
  TBranch* b_AcdTkrPointFace;

  /// DOCA between hit ACD tile and track
  Float_t m_AcdPocaDoca[2][2];
  /// Branch for AcdPocaDoca
  TBranch* b_AcdPocaDoca;

  /// ID of tile/ribbon with POCA
  Int_t m_AcdPocaTileID[2][2];
  /// Branch for AcdPocaTileID
  TBranch* b_AcdPocaTileID;

  /// Time stamp for event
  Double_t m_timeStamp;
  /// Branch for timeStamp
  TBranch* b_timeStamp;

  /// Global Position X of where track crosses ACD
  Double_t m_xAcd;

  /// Global Position Y of where track crosses ACD
  Double_t m_yAcd;

  /// Global Position Z of where track crosses ACD
  Double_t m_zAcd;

  /// Face where crosses ACD
  Int_t m_acdFace;

  /// Number of mips for PMT A for tile or ribbon with best act dist
  Double_t m_mipsPocaA;

  /// Number of mips for PMT B for tile or ribbon with best act dist
  Double_t m_mipsPocaB;

  /// Large Active Distance 
  Double_t m_actDist;

  /// ID of tile/ribbon with POCA
  Int_t m_actDistID;

  /// Output File
  TFile* m_fout;

  /// Output Tree
  TTree* m_tout;  

  /// Event index in current tree
  Long64_t m_idx;

};

#endif
