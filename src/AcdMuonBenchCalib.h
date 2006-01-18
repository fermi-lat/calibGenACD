//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Oct  4 18:23:41 2005 by ROOT version 4.02/00
// from TTree PmtTree/PmtTree
// found on file: acd_raw/Thermal_2_TrigOp_0726.root
//////////////////////////////////////////////////////////

#ifndef AcdMuonBenchCalib_h
#define AcdMuonBenchCalib_h

#include "AcdCalibBase.h"

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"

#include "AcdHistCalibMap.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <string>

class AcdMuonBenchCalib : public AcdCalibBase {

public :

  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain  

private:

  // Declaration of leave types
  Int_t           TOP[50];
  Short_t         TOP_hit[50];
  Short_t         TOP_range[50];
  Int_t           NegX[32];
  Short_t         NegX_hit[32];
  Short_t         NegX_range[32];
  Int_t           NegY[32];
  Short_t         NegY_hit[32];
  Short_t         NegY_range[32];
  Int_t           PosX[32];
  Short_t         PosX_hit[32];
  Short_t         PosX_range[32];
  Int_t           PosY[32];
  Short_t         PosY_hit[32];
  Short_t         PosY_range[32];
  
  Int_t           RIB[16];
  Short_t         RIB_hit[16];
  Short_t         RIB_range[16];
  Int_t           OTHER[22];
  Short_t         OTHER_hit[22];
  Short_t         OTHER_range[22];
  
  // List of branches
  TBranch        *b_TOP;   //!
  TBranch        *b_TOP_hit;   //!
  TBranch        *b_TOP_range;   //!
  TBranch        *b_NegX;   //!
  TBranch        *b_NegX_hit;   //!
  TBranch        *b_NegX_range;   //!
  TBranch        *b_NegY;   //!
  TBranch        *b_NegY_hit;   //!
  TBranch        *b_NegY_range;   //!
  TBranch        *b_PosX;   //!
  TBranch        *b_PosX_hit;   //!
  TBranch        *b_PosX_range;   //!
  TBranch        *b_PosY;   //!
  TBranch        *b_PosY_hit;   //!
  TBranch        *b_PosY_range;   //!
  TBranch        *b_RIB;   //!
  TBranch        *b_RIB_hit;   //!
  TBranch        *b_RIB_range;   //!
  TBranch        *b_OTHER;   //!
  TBranch        *b_OTHER_hit;   //!
  TBranch        *b_OTHER_range;   //!

public:

  AcdMuonBenchCalib(TTree *tree=0,const char* histFilename = "histograms.root");
  virtual ~AcdMuonBenchCalib();
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual void     Init(TTree *tree);
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);

protected:

  Bool_t fillGainHists(const std::set<UInt_t>& hitTiles, const std::map<UInt_t,Int_t>& hitMap);

  UInt_t localIndex(UInt_t face, UInt_t iPmt, UInt_t iRow, UInt_t iCol);

  Int_t* getAdcPtr(UInt_t face);
  Short_t* getHitPtr(UInt_t face);
  Short_t* getRangePtr(UInt_t face);

  // these is emulate the cuts in ReadActNtuple
  Bool_t applyCorrelationCut(UInt_t whichCut, const std::set<UInt_t>& hits);

  // return the total number of events in the chains
  virtual int getTotalEvents() const { 
    return fChain != 0 ? (int)(fChain->GetEntries()) : 0; 
  } 

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& /*runId*/, int& /*evtId*/);

  virtual void useEvent(Bool_t& used);

private:

  ClassDef(AcdMuonBenchCalib,0) ;

};

#endif

#ifdef AcdMuonBenchCalib_cxx

AcdMuonBenchCalib::AcdMuonBenchCalib(TTree *tree, const char* histFileName)
  :AcdCalibBase()
{
  Bool_t ok = bookHists(histFileName);
  if ( !ok ) {
    std::cerr << "ERR:  Failed to book histograms to file " << histFileName <<  std::endl;
  }  

  Init(tree);
}

AcdMuonBenchCalib::~AcdMuonBenchCalib()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();
}

Int_t AcdMuonBenchCalib::GetEntry(Long64_t entry)
{
// Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}

Long64_t AcdMuonBenchCalib::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
  if (!fChain) return -5;
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (fChain->IsA() != TChain::Class()) return centry;
  TChain *chain = (TChain*)fChain;
  if (chain->GetTreeNumber() != fCurrent) {
    fCurrent = chain->GetTreeNumber();
    Notify();
  }
  return centry;
}

void AcdMuonBenchCalib::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the branch addresses of the tree
  // will be set. It is normaly not necessary to make changes to the
  // generated code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running with PROOF.
  
  // Set branch addresses
  if (tree == 0) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);
  
  fChain->SetBranchAddress("TOP",&TOP);
  fChain->SetBranchAddress("TOP_hit",&TOP_hit);
  fChain->SetBranchAddress("TOP_range",&TOP_range);
  fChain->SetBranchAddress("NegX",&NegX);
  fChain->SetBranchAddress("NegX_hit",&NegX_hit);
  fChain->SetBranchAddress("NegX_range",&NegX_range);
  fChain->SetBranchAddress("NegY",&NegY);
  fChain->SetBranchAddress("NegY_hit",&NegY_hit);
  fChain->SetBranchAddress("NegY_range",&NegY_range);
  fChain->SetBranchAddress("PosX",&PosX);
  fChain->SetBranchAddress("PosX_hit",&PosX_hit);
  fChain->SetBranchAddress("PosX_range",&PosX_range);
  fChain->SetBranchAddress("PosY",&PosY);
  fChain->SetBranchAddress("PosY_hit",&PosY_hit);
  fChain->SetBranchAddress("PosY_range",&PosY_range);
  fChain->SetBranchAddress("RIB",&RIB);
  fChain->SetBranchAddress("RIB_hit",&RIB_hit);
  fChain->SetBranchAddress("RIB_range",&RIB_range);
  fChain->SetBranchAddress("OTHER",&OTHER);
  fChain->SetBranchAddress("OTHER_hit",&OTHER_hit);
  fChain->SetBranchAddress("OTHER_range",&OTHER_range);
  Notify();
}

Bool_t AcdMuonBenchCalib::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. Typically here the branch pointers
   // will be retrieved. It is normaly not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed.

   // Get branch pointers
  b_TOP = fChain->GetBranch("TOP");
  b_TOP_hit = fChain->GetBranch("TOP_hit");
  b_TOP_range = fChain->GetBranch("TOP_range");
  b_NegX = fChain->GetBranch("NegX");
  b_NegX_hit = fChain->GetBranch("NegX_hit");
  b_NegX_range = fChain->GetBranch("NegX_range");
  b_NegY = fChain->GetBranch("NegY");
  b_NegY_hit = fChain->GetBranch("NegY_hit");
  b_NegY_range = fChain->GetBranch("NegY_range");
  b_PosX = fChain->GetBranch("PosX");
  b_PosX_hit = fChain->GetBranch("PosX_hit");
  b_PosX_range = fChain->GetBranch("PosX_range");
  b_PosY = fChain->GetBranch("PosY");
  b_PosY_hit = fChain->GetBranch("PosY_hit");
  b_PosY_range = fChain->GetBranch("PosY_range");
  b_RIB = fChain->GetBranch("RIB");
  b_RIB_hit = fChain->GetBranch("RIB_hit");
  b_RIB_range = fChain->GetBranch("RIB_range");
  b_OTHER = fChain->GetBranch("OTHER");
  b_OTHER_hit = fChain->GetBranch("OTHER_hit");
  b_OTHER_range = fChain->GetBranch("OTHER_range");
  
  return kTRUE;
}

void AcdMuonBenchCalib::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}


UInt_t AcdMuonBenchCalib::localIndex(UInt_t face, UInt_t iPmt, UInt_t iRow, UInt_t iCol) {
  switch (face) {
  case 0: 
    return 25*iPmt + 5*iRow + iCol;
  case 1: 
  case 2:
  case 3:
  case 4: 
    return 16*iPmt + 5*iRow + iCol;
  case 5:
    return 4*iPmt + iCol;
  case 6:
    return 8 + 4*iPmt + iCol;
  case 7:
    return 11*iPmt + 10*iRow + iCol;
  default:
    ;
  }
  return 0;
}

Int_t* AcdMuonBenchCalib::getAdcPtr(UInt_t face) {
  switch (face){
  case 0: return TOP;
  case 1: return NegX;
  case 2: return NegY;
  case 3: return PosX;
  case 4: return PosY;
  case 5: 
  case 6: return RIB;
  case 7: return OTHER;
  default: ;
  }
  return 0;
}

Short_t* AcdMuonBenchCalib::getHitPtr(UInt_t face) {
  switch (face){
  case 0: return TOP_hit;
  case 1: return NegX_hit;
  case 2: return NegY_hit;
  case 3: return PosX_hit;
  case 4: return PosY_hit;
  case 5: 
  case 6: return RIB_hit;
  case 7: return OTHER_hit;
  default: ;
  }
  return 0;
}

Short_t* AcdMuonBenchCalib::getRangePtr(UInt_t face) {
  switch (face){
  case 0: return TOP_range;
  case 1: return NegX_range;
  case 2: return NegY_range;
  case 3: return PosX_range;
  case 4: return PosY_range;
  case 5: 
  case 6: return RIB_range;
  case 7: return OTHER_range;
  default: ;
  }
  return 0;
}

#endif // #ifdef AcdMuonBenchCalib_cxx
