
#ifndef AcdRangeFit_h
#define AcdRangeFit_h

#include "AcdCalibResult.h"
#include "AcdCalibMap.h"

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdRangeFitResult : public AcdCalibResult {

public:

  AcdRangeFitResult(UInt_t low, UInt_t high, STATUS status);
  AcdRangeFitResult();

  inline UInt_t low() const { return _low; }
  inline UInt_t high() const { return _high; }
  
  inline void setVals(UInt_t low, UInt_t high, STATUS status) {
    _low = low;
    _high = high;
    setStatus(status);
  }

  virtual void makeXmlNode(DomElement& node) const;
  virtual void printTxtLine(ostream& os) const;
  virtual Bool_t readTxt(istream& is);

private:
  UInt_t _low;
  UInt_t _high;
  
  ClassDef(AcdRangeFitResult,1);
};


class AcdRangeFitMap : public AcdCalibMap {
public:
  AcdRangeFitMap();
  virtual ~AcdRangeFitMap();

  virtual AcdCalibResult* createHolder() const { return new AcdRangeFitResult; }

  const AcdRangeFitResult* find(UInt_t key) const {
    return static_cast<const AcdRangeFitResult*>(get(key));
  }

  virtual const char* calibType() const {
    return "ACD_Range";
  }

  virtual const char* txtFormat() const {
    return "TILE PMT LOW HIGH STATUS";
  }

private:  

  ClassDef(AcdRangeFitMap,0) ;
};


class AcdRangeFit {

public:

  AcdRangeFit();

  virtual ~AcdRangeFit();
  

  virtual Int_t fit(AcdRangeFitResult& result, const TH1& low, const TH1& high);

  virtual Int_t fitChannel(AcdRangeFitMap& result, AcdHistCalibMap& low, AcdHistCalibMap& high, UInt_t key);

  void fitAll(AcdRangeFitMap& results, AcdHistCalibMap& lowHists, AcdHistCalibMap& highHists);

  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

private:
  
  ClassDef(AcdRangeFit,0) ;
};

#endif

#ifdef AcdRangeFit_cxx

#endif // #ifdef AcdRangeFit_cxx
