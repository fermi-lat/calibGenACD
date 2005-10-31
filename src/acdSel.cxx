#define acdSel_cxx
#include "acdSel.h"

#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>

ClassImp(acdSel) ;

void acdSel::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L acdSel.C
//      Root > acdSel t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
  if (fChain == 0) return;
  
  Long64_t nentries = fChain->GetEntriesFast();
  
  Int_t nbytes = 0, nb = 0;

  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;

    nb = fChain->GetEntry(jentry);   nbytes += nb;      
    if (Cut(ientry) < 0) continue;
    
    if ( jentry == 0 ) { ;}
    else if ( jentry % 100000 == 0 ) { std::cout << 'x' << std::endl; }
    else if ( jentry % 10000 == 0 ) { std::cout << 'x' << std::flush; }
    else if ( jentry % 1000 == 0 ) { std::cout << '.' << std::flush; }
    
    std::set<UInt_t> hitTiles;
    std::map<UInt_t,Int_t> hitMap;

    for ( UInt_t iPmt(0); iPmt < 2; iPmt++ ) {
      for ( UInt_t iFace(0); iFace < 8; iFace++ ) {
	UInt_t nRow = AcdMap::getNRow(iFace);
	
	Int_t* adcPtr = getAdcPtr(iFace);
	Short_t* hitPtr = getHitPtr(iFace);
	Short_t* rangePtr = getRangePtr(iFace);
	
	for ( UInt_t iRow(0); iRow < nRow; iRow++ ) {
	  
	  UInt_t nCol = AcdMap::getNCol(iFace,iRow);
	   
	  for ( UInt_t iCol(0); iCol < nCol; iCol++ ) {
	    
	    UInt_t iLocal = localIndex(iFace,iPmt,iRow,iCol);
	    UInt_t iTile = AcdMap::makeKey(0,0,iFace,iRow,iCol);	    

	    Int_t adcCounts = adcPtr[iLocal];
	    if ( adcCounts == 0 ) { continue; }
	    
	    Short_t rangeVal = rangePtr[iLocal];
	    
	    hitTiles.insert(iTile);
	    UInt_t iTileFull = AcdMap::makeKey(rangeVal,iPmt,iFace,iRow,iCol);	    
	  
	    hitMap[iTileFull] = adcCounts;

	    Short_t isHit = hitPtr[iLocal];
	    if ( isHit > 0 ) { continue; }	      
	    
	    if ( calType() == PEDESTAL ) {
	      fillPedestalHist(iFace, iRow, iCol, iPmt, 0, adcCounts);	   
	    }
	  }
	}
      }
    }

    if ( calType() == GAIN ) {
      fillGainHists(hitTiles,hitMap);
    }

  }
  std::cout << std::endl;
}

Bool_t acdSel::applyCorrelationCut(UInt_t whichCut, const std::set<UInt_t>& hits){

  // These sets are supposed to emulate the cuts in readACDNtuple
  // 
  // Some comments.... (or basically one comment):  FIXME
  //  There are much, much fasters ways to implement this cut, but I want to make sure that 
  //  what I have is the same thing as what is in the readAcdNtuple script and might
  //  have to change this a lot, so i'm doing it this way for now
  //

  /*
    static std::set<UInt_t> set1; // top -x-y corner
    if ( set1.size() == 0 ) {
    set1.insert(000); set1.insert(001); set1.insert(002); 
    set1.insert(010); set1.insert(011); set1.insert(012); 
    set1.insert(020); set1.insert(021); set1.insert(022); 
    }
  */
  /*
    static std::set<UInt_t> set2; // top +x-y corner
    if ( set2.size() == 0 ) {
    set2.insert(002); set2.insert(003); set2.insert(004); 
    set2.insert(012); set2.insert(013); set2.insert(014); 
    set2.insert(022); set2.insert(023); set2.insert(024);
    }
  */
  /*
    static std::set<UInt_t> set3; // top -x+y corner
    if ( set3.size() == 0 ) {
    set3.insert(020); set3.insert(021); set3.insert(022); 
    set3.insert(030); set3.insert(031); set3.insert(032); 
    set3.insert(040); set3.insert(041); set3.insert(042);
    } 
  */
  /*
    static std::set<UInt_t> set4; // top +x+y corner
    if ( set4.size() == 0 ) { 
    set4.insert(022); set4.insert(023); set4.insert(024); 
    set4.insert(032); set4.insert(033); set4.insert(034); 
    set4.insert(042); set4.insert(043); set4.insert(044);
    }
  */
  /*
    static std::set<UInt_t> set5; // -x side, -y tiles
    if ( set5.size() == 0 ) {  
    set5.insert(100); set5.insert(101); set5.insert(102); 
    set5.insert(110); set5.insert(111); set5.insert(112); 
    set5.insert(120); set5.insert(121); set5.insert(122); 
    }
  */
  /*
    static std::set<UInt_t> set6; // -x side, +y tiles
    if ( set6.size() == 0 ) {  
    set6.insert(102); set6.insert(103); set6.insert(104); 
    set6.insert(112); set6.insert(113); set6.insert(114); 
    set6.insert(122); set6.insert(123); set6.insert(124); 
    }
  */
  /*static std::set<UInt_t> set7; // -y side, -x tiles
    if ( set7.size() == 0 ) { 
    set7.insert(200); set7.insert(201); set7.insert(202); 
    set7.insert(210); set7.insert(211); set7.insert(212); 
    set7.insert(220); set7.insert(221); set7.insert(222); 
    }
  */
  /*
    static std::set<UInt_t> set8; // -y side, +x tiles
    if ( set8.size() == 0 ) { 
    set8.insert(202); set8.insert(203); set8.insert(204); 
    set8.insert(212); set8.insert(213); set8.insert(214); 
    set8.insert(222); set8.insert(223); set8.insert(224); 
    }  
  */
  /*
    static std::set<UInt_t> set9; // +x side, -y tiles
    if ( set9.size() == 0 ) { 
    set9.insert(300); set9.insert(301); set9.insert(302); 
    set9.insert(310); set9.insert(311); set9.insert(312); 
    set9.insert(320); set9.insert(321); set9.insert(322); 
    }
  */
  /*
    static std::set<UInt_t> setA; // +x side, +y tiles
    if ( setA.size() == 0 ) { 
    setA.insert(302); setA.insert(303); setA.insert(304); 
    setA.insert(312); setA.insert(313); setA.insert(314); 
    setA.insert(322); setA.insert(323); setA.insert(324); 
    }  
  */
  /*
    static std::set<UInt_t> setB; // +y side, -x tiles
    if ( setB.size() == 0 ) { 
    setB.insert(400); setB.insert(401); setB.insert(402); 
    setB.insert(410); setB.insert(411); setB.insert(412); 
    setB.insert(420); setB.insert(421); setB.insert(422); 
    }
  */
  /*
    static std::set<UInt_t> setC; // +y side, +x tiles
    if ( setC.size() == 0 ) { 
    setC.insert(402); setC.insert(403); setC.insert(404); 
    setC.insert(412); setC.insert(413); setC.insert(414); 
    setC.insert(422); setC.insert(423); setC.insert(424); 
    }   
  */ 
  /*
    static std::set<UInt_t> setD; // -x bottom
    if ( setD.size() == 0 ) {  setD.insert(130); }

    static std::set<UInt_t> setE; // -y bottom
    if ( setE.size() == 0 ) {  setD.insert(230); }
   
    static std::set<UInt_t> setF; // +x bottom 
    if ( setF.size() == 0 ) {  setF.insert(330); }

    static std::set<UInt_t> setG; // +y bottom
    if ( setG.size() == 0 ) {  setG.insert(430); }
  */

  // these are the only ones that are used
  static std::set<UInt_t> negXSide; 
  if ( negXSide.size() == 0 ) {
    negXSide.insert(100); negXSide.insert(101);  negXSide.insert(102); negXSide.insert(103); negXSide.insert(104);
    negXSide.insert(110); negXSide.insert(111);  negXSide.insert(112); negXSide.insert(113); negXSide.insert(114);
    negXSide.insert(120); negXSide.insert(121);  negXSide.insert(122); negXSide.insert(123); negXSide.insert(124);
    negXSide.insert(130);
  }

  static std::set<UInt_t> posXSide; 
  if ( posXSide.size() == 0 ) {
    posXSide.insert(300); posXSide.insert(301);  posXSide.insert(302); posXSide.insert(303); posXSide.insert(304);
    posXSide.insert(310); posXSide.insert(311);  posXSide.insert(312); posXSide.insert(313); posXSide.insert(314);
    posXSide.insert(320); posXSide.insert(321);  posXSide.insert(322); posXSide.insert(323); posXSide.insert(324);
    posXSide.insert(330);
  }

  static std::set<UInt_t> negYSide; 
  if ( negYSide.size() == 0 ) {
    negYSide.insert(200); negYSide.insert(201);  negYSide.insert(202); negYSide.insert(203); negYSide.insert(204);
    negYSide.insert(210); negYSide.insert(211);  negYSide.insert(212); negYSide.insert(213); negYSide.insert(214);
    negYSide.insert(220); negYSide.insert(221);  negYSide.insert(222); negYSide.insert(223); negYSide.insert(224);
    negYSide.insert(230);
  }

  static std::set<UInt_t> posYSide; 
  if ( posYSide.size() == 0 ) {
    posYSide.insert(400); posYSide.insert(401);  posYSide.insert(402); posYSide.insert(403); posYSide.insert(404);
    posYSide.insert(410); posYSide.insert(411);  posYSide.insert(412); posYSide.insert(413); posYSide.insert(414);
    posYSide.insert(420); posYSide.insert(421);  posYSide.insert(422); posYSide.insert(423); posYSide.insert(424);
    posYSide.insert(430);
  }

  std::list<UInt_t> output;
  std::back_insert_iterator< std::list<UInt_t> > ii(output);

  switch ( whichCut ) {
    /*
      case 1:
      std::set_intersection(set1.begin(),set1.end(),hits.begin(),hits.end(),ii);
      break;
      case 2:
      std::set_intersection(set2.begin(),set2.end(),hits.begin(),hits.end(),ii);
      break;
      case 3:
      std::set_intersection(set3.begin(),set3.end(),hits.begin(),hits.end(),ii);
      break;
      case 4:
      std::set_intersection(set4.begin(),set4.end(),hits.begin(),hits.end(),ii);
      break;
      case 5:
      std::set_intersection(set5.begin(),set5.end(),hits.begin(),hits.end(),ii);
      break;
      case 6:
      std::set_intersection(set6.begin(),set6.end(),hits.begin(),hits.end(),ii);
      break;
      case 7:
      std::set_intersection(set7.begin(),set7.end(),hits.begin(),hits.end(),ii);
      break;
      case 8:
      std::set_intersection(set8.begin(),set8.end(),hits.begin(),hits.end(),ii);
      break;
      case 9:
      std::set_intersection(set9.begin(),set9.end(),hits.begin(),hits.end(),ii);
      break;
      case 10:
      std::set_intersection(setA.begin(),setA.end(),hits.begin(),hits.end(),ii);
      break;
      case 11:
      std::set_intersection(setB.begin(),setB.end(),hits.begin(),hits.end(),ii);
      break;
      case 12:
      std::set_intersection(setC.begin(),setC.end(),hits.begin(),hits.end(),ii);
      break;
      case 13:
      std::set_intersection(setD.begin(),setD.end(),hits.begin(),hits.end(),ii);
      break;
      case 14:
      std::set_intersection(setE.begin(),setE.end(),hits.begin(),hits.end(),ii);
      break;
      case 15:
      std::set_intersection(setF.begin(),setF.end(),hits.begin(),hits.end(),ii);
      break;
      case 16:
      std::set_intersection(setG.begin(),setG.end(),hits.begin(),hits.end(),ii);
      break;
    */
  case 100:
    std::set_intersection(negXSide.begin(),negXSide.end(),hits.begin(),hits.end(),ii);
    break;
  case 200:
    std::set_intersection(negYSide.begin(),negYSide.end(),hits.begin(),hits.end(),ii);
    break;
  case 300:
    std::set_intersection(posXSide.begin(),posXSide.end(),hits.begin(),hits.end(),ii);
    break;
  case 400:
    std::set_intersection(posYSide.begin(),posYSide.end(),hits.begin(),hits.end(),ii);
    break;
  }
  return output.size() > 0;  
}


void acdSel::fillGainHists(const std::set<UInt_t>& hitTiles, const std::map<UInt_t,Int_t>& hitMap) {
  
  for ( std::map<UInt_t,Int_t>::const_iterator itr = hitMap.begin(); itr != hitMap.end(); itr++ ) {
    UInt_t key = itr->first;
    UInt_t face = AcdMap::getFace(key);
    UInt_t range = AcdMap::getRange(key);
    if ( range != 0 ) continue;
    Bool_t passesCut(kTRUE);
    switch ( face ) {
    case 1:
      passesCut = applyCorrelationCut(100,hitTiles);
      break;
    case 2:
      passesCut = applyCorrelationCut(200,hitTiles);
      break;
    case 3:
      passesCut = applyCorrelationCut(300,hitTiles);
      break;
    case 4:
      passesCut = applyCorrelationCut(400,hitTiles);
      break;    
    }
    if ( ! passesCut ) continue;
    
    UInt_t pmt = AcdMap::getPmt(key);
    UInt_t row = AcdMap::getRow(key);
    UInt_t col = AcdMap::getCol(key);
    
    fillGainHist(face,row,col,pmt,range,itr->second);
  }
}

// this should return a negative value if you don't want to use this value in the gain measurement
Float_t acdSel::correctPhaForPedestal(int /* face */, int /*row*/, int /*col*/, int /*pmtId*/, int pha) const {
  // this doesn't do pedestal subtraction but rather just returns the pha
  return (Float_t)pha;
}
