
#ifndef AcdGainFit_h
#define AcdGainFit_h

#include "AcdCalibResult.h"
#include "AcdCalibMap.h"

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdGainFitResult : public AcdCalibResult {

public:

  AcdGainFitResult(Float_t peak, Float_t width, STATUS status);
  AcdGainFitResult();

  inline Float_t peak() const { return _peak; }
  inline Float_t width() const { return _width; }
  
  inline void setVals(Float_t peak, Float_t width, STATUS status) {
    _peak = peak; _width = width;
    setStatus(status);
  }

  virtual void printXmlLine(ostream& os) const;
  virtual void printTxtLine(ostream& os) const;
  virtual Bool_t readTxt(istream& is);

private:
  Float_t _peak;
  Float_t _width;
  
  ClassDef(AcdGainFitResult,1);
};


class AcdGainFitMap : public AcdCalibMap {
public:
  AcdGainFitMap();
  virtual ~AcdGainFitMap();

  virtual AcdCalibResult* createHolder() const { return new AcdGainFitResult; }

  AcdGainFitResult* find(UInt_t key) {
    AcdCalibResult* res = get(key);
    if ( res == 0 ) return 0;
    AcdGainFitResult* fr = static_cast<AcdGainFitResult*>(res);
    return fr;
  }

  virtual const char* calibType() const {
    return "ACD_Gain";
  }

  virtual const char* txtFormat() const {
    return "TILE PMT PEAK WIDTH STATUS";
  }

private:  

  ClassDef(AcdGainFitMap,0) ;
};


class AcdGainFit {

public:

  AcdGainFit();

  virtual ~AcdGainFit();
  
  virtual Int_t fit(AcdGainFitResult& result, const TH1& hist);

  virtual Int_t fitChannel(AcdGainFitMap& result, AcdHistCalibMap& input, UInt_t key);

  void fitAll(AcdGainFitMap& results, AcdHistCalibMap& hists);

  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

private:
  
  ClassDef(AcdGainFit,0) ;
};

#endif

#ifdef AcdGainFit_cxx

#endif // #ifdef AcdGainFit_cxx
