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


class AcdGarcGafeHits {

public:

  static void convertToTilePmt(unsigned  cable, unsigned  channel, unsigned& tile, unsigned& pmt);

public:
  
  AcdGarcGafeHits();
  virtual ~AcdGarcGafeHits();

  
  // setters
  void reset();

  void setDigis(const TObjArray& digs);
  void setDigi(const AcdDigi& digi);

  void setHits(const AcdRecon& recon);
  void setHit(const AcdHit& hit);

  inline void setCNO(const UShort_t& cno) {
    _cno = cno;
  }

  // Access
  void garcStatus(UInt_t garc, Bool_t& cno, UInt_t& nHits, UInt_t& nVeto) const;
  Bool_t nextGarcHit(UInt_t garc, Int_t& gafe);
  Bool_t nextGarcVeto(UInt_t garc, Int_t& gafe);

  Int_t findCno_oneHit(UInt_t garc);
  Int_t findCno_oneVeto(UInt_t garc);
  

  inline Float_t inMips(UInt_t garc, UInt_t gafe) const {
    return _inMips[garc][gafe];
  }
  inline UShort_t inPha(UInt_t garc, UInt_t gafe) const {
    return _inPha[garc][gafe];
  }
  inline UShort_t flags(UInt_t garc, UInt_t gafe) const {
    return _flags[garc][gafe];
  }
  inline UShort_t nHits(UInt_t garc) const {
    return _nHits[garc];
  }
  inline UShort_t nVeto(UInt_t garc) const {
    return _nVeto[garc];
  }
  inline UShort_t cno() const {
    return _cno;
  }

protected:

  void lookup(const AcdId& id, UInt_t pmt, UInt_t& garc, UInt_t& gafe);

private:  

  Float_t  _inMips[12][18];
  UShort_t _inPha[12][18];
  UShort_t _flags[12][18];  
  UShort_t _nHits[12];
  UShort_t _nVeto[12];
  UShort_t _cno;
  
};

#endif

#ifdef AcdGarcGafeHits_cxx

#endif // #ifdef AcdGarcGafeHits_cxx
