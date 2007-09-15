#define AcdCalibLoop_Bench_cxx
#include "AcdCalibLoop_Bench.h"

#include "AcdCalibMap.h"
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>

ClassImp(AcdCalibLoop_Bench) ;

Bool_t AcdCalibLoop_Bench::applyCorrelationCut(UInt_t whichCut, const std::set<UInt_t>& hits){

  // These sets are supposed to emulate the cuts in readACDNtuple
  // 
  // Some comments.... (or basically one comment):  FIXME
  //  There are much, much fasters ways to implement this cut, but I want to make sure that 
  //  what I have is the same thing as what is in the readAcdNtuple script and might
  //  have to change this a lot, so i'm doing it this way for now
  //

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


Bool_t AcdCalibLoop_Bench::fillGainHists(const std::set<UInt_t>& hitTiles, const std::map<UInt_t,Int_t>& hitMap) {

  Bool_t retVal(kFALSE);
  for ( std::map<UInt_t,Int_t>::const_iterator itr = hitMap.begin(); itr != hitMap.end(); itr++ ) {
    UInt_t key = itr->first;
    UInt_t id = AcdMap::getId(key);
    UInt_t face = AcdMap::getFace(key);
    UInt_t pmt = AcdMap::getPmt(key);
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
    retVal = kTRUE;
    fillHist(*m_gainHists,id,pmt,(Float_t)(itr->second) );
  }
  return retVal;
}

Bool_t AcdCalibLoop_Bench::readEvent(int ievent, Bool_t& filtered, 
				    int& /*runId*/, int& /*evtId*/) {

  filtered = kFALSE;
  Long64_t ientry = LoadTree(ievent);
  if (ientry < 0) return kFALSE;

  int nb = fChain->GetEntry(ientry);
  if ( nb == 0 ) return kFALSE;
  return kTRUE;
}

void AcdCalibLoop_Bench::useEvent(Bool_t& used) {

  used = kFALSE;
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
	  UInt_t iTile = AcdMap::makeId(iFace,iRow,iCol);	    
	  
	  Int_t adcCounts = adcPtr[iLocal];
	  if ( adcCounts == 0 ) { continue; }
	  
	  Short_t rangeVal = rangePtr[iLocal];
	  if ( rangeVal != 0 ) continue;
	  
	  hitTiles.insert(iTile);
	  UInt_t iTileFull = AcdMap::makeKey(iPmt,iFace,iRow,iCol);	    
	  
	  hitMap[iTileFull] = adcCounts;
	  
	  Short_t isHit = hitPtr[iLocal];
	  if ( isHit > 0 ) { continue; }	      
	  
	  if ( calType() == AcdCalib::PEDESTAL ) {
	    fillHist(*m_pedHists,iTile, iPmt, adcCounts);	   
	  }
	}
      }
    }
  }
  
  if ( calType() == AcdCalib::GAIN ) {
    used = fillGainHists(hitTiles,hitMap);
  }

}
