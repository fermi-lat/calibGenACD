#ifndef AcdVetoCal_h
#define AcdVetoCal_h

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdVetoCalResult {

public:

  enum STATUS { NOFIT = -1,
		OK = 0,	 
		FAILED = 1 };

public:

  AcdVetoCalResult(Float_t offset, Float_t slope, Float_t chisq, STATUS status);
  AcdVetoCalResult();

  inline Float_t vetoDac(Float_t tci) const {
    return _offset + _slope*tci;
  }

  inline Float_t offset() const { return _offset; }
  inline Float_t slope() const { return _slope; }
  inline Float_t chisq() const { return _chisq; }
  inline STATUS status() const { return _status; }
  
  inline void setVals(Float_t offset, Float_t slope, Float_t chisq, STATUS status) {
    _offset = offset; _slope = slope; _chisq = chisq; _status = status;
  }

private:

  Float_t _offset;
  Float_t _slope;
  Float_t _chisq;
  STATUS _status;
  
  ClassDef(AcdVetoCalResult,0);
};


#endif

#ifdef AcdVetoCal_cxx

#endif // #ifdef AcdVetoCal_cxx
