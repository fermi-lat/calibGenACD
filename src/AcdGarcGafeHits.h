#ifndef AcdGarcGafeHits_h
#define AcdGarcGafeHits_h

// ROOT includes
#include <Rtypes.h>

// forward declares
class TObjArray;
class AcdDigi;
class AcdHit;
class AcdRecon;
class AcdId;

/** 
 * @class AcdGarcGafeHits
 *
 * @brief Tool to sort and count hits by GARC and GAFE
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdGarcGafeHits.h,v 1.7 2008/08/01 00:08:25 echarles Exp $
 */

class AcdGarcGafeHits {

public:

  /// Go from GARC, GAFE to tile and PMT
  static void convertToTilePmt(unsigned  cable, unsigned  channel, unsigned& tile, unsigned& pmt);

  /// Go from tile and pmt to garc and gafe 
  static void convertToGarcGafe(unsigned  tile, unsigned  pmt, unsigned& garc, unsigned& gafe);
  

public:
  
  AcdGarcGafeHits();
  virtual ~AcdGarcGafeHits();

  
  /// reset the counters
  void reset();

  /// count up all the digis
  void setDigis(const TObjArray& digs);
  /// count one digi
  void setDigi(const AcdDigi& digi, Float_t pmtA, Float_t pmtB);

  /// count up all the hits
  void setHit(Int_t id, Float_t mipsA, Float_t mipsB);

  /// set the CNO mask
  inline void setCNO(const UShort_t& cno) {
    _cno = cno;
  }

  /// return the CNO status, number of hits and vetos in one GARC
  void garcStatus(UInt_t garc, Bool_t& cnoSet, UInt_t& nHits, UInt_t& nVeto) const;
  /// scan through hits in a GARC
  Bool_t nextGarcHit(UInt_t garc, Int_t& gafe);
  /// scan through vetos in a GARC
  Bool_t nextGarcVeto(UInt_t garc, Int_t& gafe);

  /// look for GARC with CNO and exactly 1 hit  
  Int_t findCno_oneHit(UInt_t garc);
  /// look for GARC with CNO and exactly 1 veto  
  Int_t findCno_oneVeto(UInt_t garc);
  
  
  /// return the signal size by in mips by garc:gafe
  inline Float_t inMips(UInt_t garc, UInt_t gafe) const {
    return _inMips[garc][gafe];
  }
  /// return the signal size by in PHA by garc:gafe
  inline UShort_t inPha(UInt_t garc, UInt_t gafe) const {
    return _inPha[garc][gafe];
  }
  /// return the flags by garc:gafe
  inline UShort_t flags(UInt_t garc, UInt_t gafe) const {
    return _flags[garc][gafe];
  }
  /// return the number of hits in a garc
  inline UShort_t nHits(UInt_t garc) const {
    return _nHits[garc];
  }
  /// return the number of vetos in a garc
  inline UShort_t nVeto(UInt_t garc) const {
    return _nVeto[garc];
  }
  /// return the CNO mask
  inline UShort_t cno() const {
    return _cno;
  }

protected:

  /// Lookup the garc, gafe given and AcdId and PMT
  void lookup(const AcdId& id, UInt_t pmt, UInt_t& garc, UInt_t& gafe);

private:  

  /// Signal sizes in mips
  Float_t  _inMips[12][18];
  /// Signal sizes in PHA
  UShort_t _inPha[12][18];
  /// Discrimiator and range flags
  UShort_t _flags[12][18];  
  /// number of hits in a garc
  UShort_t _nHits[12];
  /// number of vetos in a garc
  UShort_t _nVeto[12];
  /// the CNO mask
  UShort_t _cno;
  
};

#endif
