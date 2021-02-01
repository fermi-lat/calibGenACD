
#ifndef AcdKey_calibGenACD_h
#define AcdKey_calibGenACD_h

// ROOT includes
#include "Rtypes.h"

// stl includes
#include <iostream>
#include <string>
#include <list>

/** 
 * @class AcdKey
 *
 * @brief Various tools to associate map tile into a unique key and back
 *  
 * All mapping is done using a decimal key:
 *    1000 * pmt + 100*face + 10*row + col
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdKey.h,v 1.3 2008/09/05 22:57:50 echarles Exp $
 */

class AcdKey {

public:

  enum ChannelSet { All = 0, 
		    Tiles = 1,
		    NoSkirt = 2,
		    Ribbons = 3 };

  enum { nPmt = 2 } PMT;
  enum { nFace = 8 } FACE;
  enum { nRow = 5 } ROW;
  enum { nCol = 5 } COLUMN;

  enum Config{ LAT,
	       GARC_GAFE,
	       RIBBONS,
	       BEAM };  

  
  /// Check to see if we should use a channel
  static Bool_t useChannel(UInt_t id, ChannelSet cSet);

  static UInt_t makeId(UInt_t face, UInt_t row, UInt_t col) {
    return 100*face + 10*row + col;
  }
  static UInt_t makeKey(UInt_t pmt, UInt_t face, UInt_t row, UInt_t col, UInt_t iHist=0) {
    return (10000* iHist) + (1000 * pmt) + makeId(face,row,col);
  }
  static UInt_t makeKey(UInt_t pmt, UInt_t id) {
    return 1000 * pmt + id;
  }
  static UInt_t getId(UInt_t key) {
    return key % 1000;
  }
  static UInt_t getPmt(UInt_t key) {
    return (key % 10000 ) / 1000;
  }
  static UInt_t getFace(UInt_t key) {
    return (key % 1000) / 100;
  }
  static UInt_t getRow(UInt_t key) {
    return (key % 100) / 10;
  }
  static UInt_t getCol(UInt_t key) {
    return (key % 10);
  }  

  static UInt_t getNRow(UInt_t face);
  
  static UInt_t getNCol(UInt_t face,UInt_t row);

  static Bool_t channelExists(int id) {
    return channelExists( getFace(id), getRow(id), getCol(id) );
  }
  static Bool_t channelExists(int iFace,int iRow,int iCol);

  static void makeSuffix(std::string& suffix, UInt_t iPmt, UInt_t iFace, UInt_t iRow, UInt_t iCol, UInt_t idx);
  
  static void nSpaces(std::ostream& os, int n);  

  static std::list<Int_t>& acdIdList();

public:

  AcdKey( ){;};
  virtual ~AcdKey(){;};

protected:

private:

};

#endif

#ifdef AcdKey_calibGenACD_cxx

#include <sstream>

UInt_t AcdKey::getNRow(UInt_t face) {
  switch (face) {
  case 0: 
    return 5;
  case 1: 
  case 2:
  case 3:
  case 4:
    return 4;
  case 5:
  case 6:
    return 1;
  case 7:
    return 2;
  default:
    ;
  }
  return 0;
}
  
UInt_t AcdKey::getNCol(UInt_t face,UInt_t row) {
  switch (face) {
  case 0: 
    return 5;
  case 1: 
  case 2:
  case 3:
  case 4:
    return row == 3 ? 1 : 5;
  case 5:
  case 6:
    return 4;
  case 7:
    return row == 1 ? 2 : 10;
  default:
    ;
  }
  return 0;
}


Bool_t AcdKey::channelExists(int iFace,int iRow,int iCol) {
  switch (iFace) {
  case 0:
    // top
    if ( iRow > 4 || iCol > 4 ) return false;
    break;
  case 1:
  case 2:
  case 3:
  case 4:
    if ( iRow > 3 ) return false;
    if ( iRow == 3 && iCol > 0 ) return false;
    break;
  case 5:
  case 6:
    if ( iRow > 0 ) return false;
    if ( iCol > 3 ) return false;
    break;
  case 7:
    if ( iRow * 10 + iCol > 10 ) return false;
    break;    
  default:
    return false;
    break;
  }
  return true;
}

void AcdKey::makeSuffix(std::string& suffix, UInt_t iPmt, UInt_t iFace, UInt_t iRow, UInt_t iCol, UInt_t idx) {
  std::stringstream suf;
  suf << iPmt << '_' << iFace << iRow << iCol;
  if ( idx > 0 ) {
    suf << '_' << idx;
  }
  suffix = suf.str();
}

void AcdKey::nSpaces(std::ostream& os, int n) {
  for ( int i(0); i < n; i++ ) {
    os << ' ';
  }
}
 
#endif
