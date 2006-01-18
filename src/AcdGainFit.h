
#ifndef AcdGainFit_h
#define AcdGainFit_h

#include "AcdCalibResult.h"
#include "AcdCalibMap.h"

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdGainFitResult : public AcdCalibResult {

public:

  AcdGainFitResult(Float_t peak, Float_t width, STATUS status, Int_t type);
  AcdGainFitResult();

  inline Float_t peak() const { return _peak; }
  inline Float_t width() const { return _width; }
  inline Int_t type() const { return _type; }
  
  inline void setVals(Float_t peak, Float_t width, STATUS status, Int_t type) {
    _peak = peak; _width = width; _type = type;
    setStatus(status);
  }

  virtual void printXmlLine(ostream& os) const;
  virtual void printTxtLine(ostream& os) const;
  virtual Bool_t readTxt(istream& is);

private:
  Float_t _peak;
  Float_t _width;
  Int_t _type;
  
  ClassDef(AcdGainFitResult,1);
};


class AcdGainFitMap : public AcdCalibMap {
public:
  AcdGainFitMap();
  virtual ~AcdGainFitMap();

  virtual AcdCalibResult* createHolder() const { return new AcdGainFitResult; }

  virtual const char* calibType() {
    return "ACD_ElecGain";
  }

  virtual const char* txtFormat() {
    return "TILE PMT PEAK WIDTH STATUS TYPE";
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

private:
  
  ClassDef(AcdGainFit,0) ;
};

#endif

#ifdef AcdGainFit_cxx

#endif // #ifdef AcdGainFit_cxx
