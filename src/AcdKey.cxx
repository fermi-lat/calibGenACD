#define AcdKey_calibGenACD_cxx
#include "AcdKey.h"



Bool_t AcdKey::useChannel(UInt_t id, ChannelSet cSet) {
  if ( id >= 700 ) return kFALSE;
  if ( id >= 500 ) {
    // ribbons
    switch ( cSet ) {
    case All:
    case Ribbons:
      return kTRUE;
    case Tiles:
    case NoSkirt:
      return kFALSE;
    }
  }
  if ( id < 100 ) {
    // top
    switch ( cSet ) {
    case All:
    case Tiles:
    case NoSkirt:
      return kTRUE;
    case Ribbons:
      return kFALSE;
    }
  }
  if ( (id%100) >= 20 ) {
    // skirt
    switch ( cSet ) {
    case All:
    case Tiles:
      return kTRUE;
    case NoSkirt:    
    case Ribbons:
      return kFALSE;
    }    
  }
  // side, but not in skirt
  switch ( cSet ) {
  case All:
  case Tiles:
  case NoSkirt:    
    return kTRUE;
  case Ribbons:
    return kFALSE;
  }    
  return kFALSE;
}

std::list<Int_t>& AcdKey::acdIdList() {
  static std::list<Int_t> theAcdIdList;
  if ( theAcdIdList.size() == 0 ) {
    // Top, 0-4, 10-14, 20-24, 30-34, 40-44
    theAcdIdList.push_back(0); 
    theAcdIdList.push_back(1); 
    theAcdIdList.push_back(2); 
    theAcdIdList.push_back(3); 
    theAcdIdList.push_back(4); 
    theAcdIdList.push_back(10); 
    theAcdIdList.push_back(11); 
    theAcdIdList.push_back(12); 
    theAcdIdList.push_back(13); 
    theAcdIdList.push_back(14); 
    theAcdIdList.push_back(20); 
    theAcdIdList.push_back(21); 
    theAcdIdList.push_back(22); 
    theAcdIdList.push_back(23); 
    theAcdIdList.push_back(24); 
    theAcdIdList.push_back(30); 
    theAcdIdList.push_back(31); 
    theAcdIdList.push_back(32); 
    theAcdIdList.push_back(33); 
    theAcdIdList.push_back(34); 
    theAcdIdList.push_back(40); 
    theAcdIdList.push_back(41); 
    theAcdIdList.push_back(42); 
    theAcdIdList.push_back(43); 
    theAcdIdList.push_back(44); 
    // -X side, 100-104, 110-114, 120-124, 130
    theAcdIdList.push_back(100); 
    theAcdIdList.push_back(101); 
    theAcdIdList.push_back(102); 
    theAcdIdList.push_back(103); 
    theAcdIdList.push_back(104); 
    theAcdIdList.push_back(110); 
    theAcdIdList.push_back(111); 
    theAcdIdList.push_back(112); 
    theAcdIdList.push_back(113); 
    theAcdIdList.push_back(114); 
    theAcdIdList.push_back(120); 
    theAcdIdList.push_back(121); 
    theAcdIdList.push_back(122); 
    theAcdIdList.push_back(123); 
    theAcdIdList.push_back(124); 
    theAcdIdList.push_back(130); 
    // -Y side, 200-204, 210-214, 220-224, 230
    theAcdIdList.push_back(200); 
    theAcdIdList.push_back(201); 
    theAcdIdList.push_back(202); 
    theAcdIdList.push_back(203); 
    theAcdIdList.push_back(204); 
    theAcdIdList.push_back(210); 
    theAcdIdList.push_back(211); 
    theAcdIdList.push_back(212); 
    theAcdIdList.push_back(213); 
    theAcdIdList.push_back(214); 
    theAcdIdList.push_back(220); 
    theAcdIdList.push_back(221); 
    theAcdIdList.push_back(222); 
    theAcdIdList.push_back(223); 
    theAcdIdList.push_back(224); 
    theAcdIdList.push_back(230); 
    // +X side, 300-304, 310-314, 320-324, 330
    theAcdIdList.push_back(300); 
    theAcdIdList.push_back(301); 
    theAcdIdList.push_back(302); 
    theAcdIdList.push_back(303); 
    theAcdIdList.push_back(304); 
    theAcdIdList.push_back(310); 
    theAcdIdList.push_back(311); 
    theAcdIdList.push_back(312); 
    theAcdIdList.push_back(313); 
    theAcdIdList.push_back(314); 
    theAcdIdList.push_back(320); 
    theAcdIdList.push_back(321); 
    theAcdIdList.push_back(322); 
    theAcdIdList.push_back(323); 
    theAcdIdList.push_back(324); 
    theAcdIdList.push_back(330); 
    // +Y side, 400-404, 410-414, 420-424, 430
    theAcdIdList.push_back(400); 
    theAcdIdList.push_back(401); 
    theAcdIdList.push_back(402); 
    theAcdIdList.push_back(403); 
    theAcdIdList.push_back(404); 
    theAcdIdList.push_back(410); 
    theAcdIdList.push_back(411); 
    theAcdIdList.push_back(412); 
    theAcdIdList.push_back(413); 
    theAcdIdList.push_back(414); 
    theAcdIdList.push_back(420); 
    theAcdIdList.push_back(421); 
    theAcdIdList.push_back(422); 
    theAcdIdList.push_back(423); 
    theAcdIdList.push_back(424); 
    theAcdIdList.push_back(430); 
    // Ribbons running from -Y to +Y, 500-501, not need on TOP of ACD
    theAcdIdList.push_back(500); 
    theAcdIdList.push_back(501); 
    theAcdIdList.push_back(502); 
    theAcdIdList.push_back(503); 
    // Ribbons running from -Y to +Y, 600-601
    theAcdIdList.push_back(600); 
    theAcdIdList.push_back(601); 
    theAcdIdList.push_back(602); 
    theAcdIdList.push_back(603); 
  }
  return theAcdIdList;
}
