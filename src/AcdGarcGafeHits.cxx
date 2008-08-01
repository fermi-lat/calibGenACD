#define AcdGarcGafeHits_cxx

#include "AcdGarcGafeHits.h"


#include "reconRootData/ReconEvent.h"
#include "reconRootData/AcdHit.h"
#include "digiRootData/AcdDigi.h"
#include "commonRootData/AcdMap.h"

#include <iostream>
#include <fstream>

#include <assert.h>


void AcdGarcGafeHits::convertToTilePmt(unsigned  cable, unsigned  channel, unsigned& tile, unsigned& pmt) {
  AcdMap::convertToTilePmt(cable,channel,tile,pmt);
}

void AcdGarcGafeHits::convertToGarcGafe(unsigned  tile, unsigned  pmt, unsigned& garc, unsigned& gafe) {
  AcdMap::convertToGarcGafe(tile,pmt,garc,gafe);
}

AcdGarcGafeHits::AcdGarcGafeHits(){
  reset();
}

AcdGarcGafeHits::~AcdGarcGafeHits() {;}


void AcdGarcGafeHits::reset() {
  _cno = 0;
  for ( UInt_t iGarc(0); iGarc < 12; iGarc++ ){
    _nHits[iGarc] = 0;
    _nVeto[iGarc] = 0;
    for ( UInt_t iGafe(0); iGafe < 18; iGafe++ ) {
      _inMips[iGarc][iGafe] = 0.;
      _inPha[iGarc][iGafe] = 0;
      _flags[iGarc][iGafe] = 0;      
    }
  }
}


void AcdGarcGafeHits::setDigis(const TObjArray& digis){
  int nAcdDigi = digis.GetLast() + 1;
  for(int j = 0; j != nAcdDigi; j++) {      
    const AcdDigi* acdDigi = static_cast<const AcdDigi*>(digis.At(j));      
    setDigi(*acdDigi,(Float_t)acdDigi->getPulseHeight(AcdDigi::A),(Float_t)acdDigi->getPulseHeight(AcdDigi::B));
  }
}

void AcdGarcGafeHits::setDigi(const AcdDigi& digi, Float_t pmtA, Float_t pmtB){
  const AcdId& acdId = digi.getId();

  UInt_t garc(13), gafe(19);

  lookup(acdId,0,garc,gafe);  
  if ( digi.getAcceptMapBit(AcdDigi::A) ) {
    _flags[garc][gafe] |= 0x1;
    _nHits[garc] += 1;
  }
  if ( digi.getHitMapBit(AcdDigi::A) ) {
    _flags[garc][gafe] |= 0x2;
    _nVeto[garc] += 1;
  }
  if ( digi.getRange(AcdDigi::A) == AcdDigi::HIGH ){
    _flags[garc][gafe] |= 0x4;
  }
  if ( pmtA > 0) {
    _inPha[garc][gafe] = (UInt_t)pmtA;
  }
  
  lookup(acdId,1,garc,gafe);
  _inPha[garc][gafe] = digi.getPulseHeight(AcdDigi::B);
  if ( digi.getAcceptMapBit(AcdDigi::B) ) {
    _flags[garc][gafe] |= 0x1;
    _nHits[garc] += 1;
  }      
  if ( digi.getHitMapBit(AcdDigi::B) ) {
    _flags[garc][gafe] |= 0x2;
    _nVeto[garc] += 1;
  } 
  if ( digi.getRange(AcdDigi::B) == AcdDigi::HIGH ) {
    _flags[garc][gafe] |= 0x4;
  }
  if ( pmtB > 0 ) {
    _inPha[garc][gafe] = (UInt_t)pmtB;
  }
}



void AcdGarcGafeHits::setHit(Int_t id, Float_t mipsA, Float_t mipsB) {
  UInt_t garcA(0), gafeA(0), garcB(0), gafeB(0);
  convertToGarcGafe(id,0,garcA,gafeA);
  convertToGarcGafe(id,1,garcB,gafeB);

  _inMips[garcA][gafeA] = mipsA;
  _inMips[garcB][gafeB] = mipsB;

  if ( mipsA > 0. )  {
    _nHits[garcA] += 1;
    _nVeto[garcA] += 1;
    _flags[garcA][gafeA] += 0x3;
  }
  if ( mipsB > 0. ) { 
    _nHits[garcB] += 1;
    _nVeto[garcB] += 1;
    _flags[garcB][gafeB] += 0x3;
  }
}


void AcdGarcGafeHits::garcStatus(UInt_t garc, Bool_t& cnoSet, UInt_t& nHits, UInt_t& nVeto) const {  
  cnoSet = (_cno & ( 0x1 << garc )) != 0;  
  nHits = _nHits[garc];
  nVeto = _nVeto[garc];
  //if ( cnoSet || ( nVeto == 1 ) ) {
  //std::cout << garc << ' ' << cnoSet << ' ' << nHits << ' ' << nVeto << std::endl;
  //}
}

Bool_t AcdGarcGafeHits::nextGarcHit(UInt_t garc, Int_t& gafe) {
  while ( gafe < 17 ) {
    gafe++;
    if ( _flags[garc][gafe] & 0x1 ) return kTRUE;
  }
  return kFALSE;
}

Bool_t AcdGarcGafeHits::nextGarcVeto(UInt_t garc, Int_t& gafe) {
  while ( gafe < 17 ) {
    gafe++;
    if ( _flags[garc][gafe] & 0x2) return kTRUE;
  }
  return kFALSE;
}

Int_t AcdGarcGafeHits::findCno_oneHit(UInt_t garc) {
  Int_t retVal(-1);
  if ( _nHits[garc] != 1 ) return retVal;
  if ( nextGarcHit(garc,retVal) ) return retVal;
  return -1;
}

Int_t AcdGarcGafeHits::findCno_oneVeto(UInt_t garc) {
  Int_t retVal(-1);
  if ( _nVeto[garc] != 1 ) return retVal;
  if ( nextGarcVeto(garc,retVal) ) return retVal;
  return -1;
}

void AcdGarcGafeHits::lookup(const AcdId& id, UInt_t pmt, UInt_t& garc, UInt_t& gafe) {  
  AcdMap::convertToGarcGafe(id.getId(),pmt,garc,gafe);
}
